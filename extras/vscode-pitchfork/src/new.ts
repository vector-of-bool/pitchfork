/**
 * Module concerning the generation of new Pitchfork projects
 */ /** */

import * as path from 'path';
import * as vscode from 'vscode';

import {createDirectories} from './dirs';
import {fs} from './pr';
import { createCMakeFiles } from './cmake';


enum WizardState {
  GetName = 1,
  GetRootNamespace,
  GetBuildSystem,
  GetOtherFlags,
  Finished,
  Cancelled,
}

enum NewProjectFlags {
  None = 0,
  GenerateThirdParty = 1 << 1,
  SeparateHeaders = 1 << 2,
  GenerateExamples = 1 << 3,
}

export enum BuildSystem {
  None = 0,
  CMake = 1,
}

export interface NewProjectParams {
  name: string;
  rootNamespace: string;
  buildSystem: BuildSystem;
  generateThirdParty: boolean;
  generateExamples: boolean;
  separateHeaders: boolean;
}

type Awaitable<T> = T|Thenable<T>;

class NewProjectWizard {
  constructor(readonly baseDir: string) {}
  private _state: WizardState = WizardState.GetName;

  // Parameters for the new project.
  private _name = '';
  private _rootNamespace = '';
  private _buildSystem = BuildSystem.None;
  private _otherFlags = NewProjectFlags.None;

  async runToCompletion(): Promise<NewProjectParams|undefined> {
    while (true) {
      await this._runNext();
      if (this._state === WizardState.Finished) {
        break;
      }
      if (this._state === WizardState.Cancelled) {
        return undefined;
      }
    }
    return {
      name: this._name,
      rootNamespace: this._rootNamespace,
      buildSystem: this._buildSystem,
      generateThirdParty: !!(this._otherFlags & NewProjectFlags.GenerateThirdParty),
      generateExamples: !!(this._otherFlags & NewProjectFlags.GenerateExamples),
      separateHeaders: !!(this._otherFlags & NewProjectFlags.SeparateHeaders),
    };
  }

  private _runNext() {
    switch (this._state) {
    case WizardState.GetName:
      return this._getName();
    case WizardState.GetRootNamespace:
      return this._getRootNamespace();
    case WizardState.GetBuildSystem:
      return this._getBuildSystem();
    case WizardState.GetOtherFlags:
      return this._getOtherFlags();
    default:
      debugger;
      console.assert(false, 'Bad wizard state', this._state);
      throw new Error('Bad wizard');
    }
  }

  private _initQuickInput(input: vscode.QuickInput, title: string) {
    input.title = title;
    input.step = this._state as number;
    input.totalSteps = this._state as number;
    input.ignoreFocusOut = true;
  }

  private _createInputBox(name: string, prompt: string): vscode.InputBox {
    const box = vscode.window.createInputBox();
    this._initQuickInput(box, name);
    box.prompt = prompt;
    return box;
  }

  private _createQuickPick<T extends vscode.QuickPickItem>(name: string): vscode.QuickPick<T> {
    const pick = vscode.window.createQuickPick<T>();
    this._initQuickInput(pick, name);
    return pick;
  }

  private async _getInputString(
      opt: {
        title: string,
        prompt: string,
        defaultValue?: string, validate?(value: string): Awaitable<string|undefined>,
      },
      ): Promise<string|undefined> {
    const box = this._createInputBox(opt.title, opt.prompt);
    if (opt.defaultValue) {
      box.value = opt.defaultValue;
    }
    try {
      let resolved = false;
      return await new Promise<string|undefined>(resolve => {
        box.onDidAccept(async () => {
          if (opt.validate) {
            box.busy = true;
            box.validationMessage = await opt.validate(box.value);
            box.busy = false;
          }
          if (box.validationMessage) {
            return;
          }
          resolved = true;
          resolve(box.value);
        });
        box.onDidChangeValue(async value => {
          if (opt.validate) {
            box.busy = true;
            box.validationMessage = await opt.validate(value);
            box.busy = false;
          }
        });
        box.onDidHide(() => {
          if (!resolved) {
            resolve(undefined);
          }
        });
        box.show();
      });
    } finally {
      // Close the box
      box.dispose();
    }
  }

  private async _getName() {
    const name = await this._getInputString({
      title: 'Project Name',
      prompt: 'Enter the name for your new project',
      validate: async newName => {
        if (newName === '') {
          return 'A project name is required';
        }
        const newDirPath = path.join(this.baseDir, newName);
        const stat = await fs.tryStat(newDirPath);
        if (!stat) {
          return undefined;
        } else if (stat.isDirectory()) {
          return 'A project with this name already exists';
        } else {
          return 'A file in the project directory already has this name';
        }
      },
    });
    if (!name) {
      // User cancelled input
      this._state = WizardState.Cancelled;
    } else {
      this._name = name;
      this._state++;
    }
  }

  private async _getRootNamespace() {
    const ns = await this._getInputString({
      title: 'Root Namespace',
      prompt: 'Enter the base root namespace for the new project',
      defaultValue: this._name,
      validate: newNS => {
        const items = newNS.split('::');
        const badItem = items.find(elem => !/^[a-zA-Z_][a-zA-Z0-9_]*$/.test(elem));
        if (badItem || newNS.endsWith('::')) {
          return 'Invalid C++ namespace';
        }
        return;
      }
    });
    if (!ns) {
      this._state = WizardState.Cancelled;
    } else {
      this._rootNamespace = ns;
      this._state++;
    }
  }

  private async _getQuickPick<T extends vscode.QuickPickItem>(opt: {
    title: string,
    items: T[],
    canSelectMany?: boolean,
  }): Promise<T[]|undefined> {
    const pick = this._createQuickPick<T>(opt.title);
    if (opt.canSelectMany) {
      pick.canSelectMany = true;
    }
    pick.items = opt.items;
    try {
      let resolved = false;
      return await new Promise<T[]|undefined>(resolve => {
        pick.onDidAccept(() => {
          if (!pick.selectedItems) {
            return;
          }
          resolved = true;
          resolve([...pick.selectedItems]);
        });
        pick.onDidHide(() => {
          if (!resolved) {
            resolve(undefined);
          }
        });
        pick.show();
      });
    } finally {
      // Dispose of the page
      pick.dispose();
    }
  }

  private async _getBuildSystem() {
    interface BuildSystemItems {
      label: string;
      detail: string;
      buildSystem: BuildSystem;
    }
    const chosen = await this._getQuickPick<BuildSystemItems>({
      title: 'Build System',
      items: [
        {
          label: 'CMake',
          detail: 'Generate a CMake build system in the new project',
          buildSystem: BuildSystem.CMake,
        },
        {
          label: 'None',
          detail: 'Do not generate a build system',
          buildSystem: BuildSystem.None,
        },
      ],
    });
    if (!chosen) {
      this._state = WizardState.Cancelled;
    } else {
      this._state++;
      this._buildSystem = chosen[0].buildSystem;
    }
  }

  private async _getOtherFlags() {
    interface OtherOptionItems {
      label: string;
      detail: string;
      flag: NewProjectFlags;
      picked: boolean;
    }
    const chosen = await this._getQuickPick<OtherOptionItems>({
      title: 'Other Options',
      canSelectMany: true,
      items: [
        {
          label: 'Separate Headers and Sources',
          detail: 'Put header files in a separate `include/` directory',
          flag: NewProjectFlags.SeparateHeaders,
          picked: false,
        },
        {
          label: 'Generate a third_party/ directory',
          detail: 'Create a third_party/ directory for external libraries',
          flag: NewProjectFlags.GenerateThirdParty,
          picked: false,
        },
        {
          label: 'Generate an examples/ directory',
          detail: 'Create an examples/ directory for library/program example usages',
          flag: NewProjectFlags.GenerateExamples,
          picked: true,
        },
      ],
    });
    if (!chosen) {
      this._state = WizardState.Cancelled;
    } else {
      this._otherFlags = chosen.reduce((acc, item) => acc | item.flag, NewProjectFlags.None);
      this._state++;
    }
  }
}

/**
 * Show the new project UI and accept input from the user. Returns the parameters
 * the user requested from the UI.
 * @param baseDir The directory where the project may be placed (only used for validation)
 */
export function getNewProjectParametersFromUI(baseDir: string): Promise<NewProjectParams|undefined> {
  const wiz = new NewProjectWizard(baseDir);
  return wiz.runToCompletion();
}

/**
 * Create a new Pitchfork file structure on disk.
 * @param baseDir The directory in which to create the project
 * @param params The parameters to use when creating the project
 */
export async function createNewProject(baseDir: string, params: NewProjectParams): Promise<vscode.Uri> {
  await createDirectories(baseDir, params);
  const prDir = path.join(baseDir, params.name);
  if (params.buildSystem === BuildSystem.CMake) {
    await createCMakeFiles(prDir, params);
  }
  // Since we will be opening the project in a new VSCode window, we will lose
  // the parameters used to create the project. Write those parameters to a file
  // that will be loaded by the extension immediately when the project is opened
  // so that we can continue as usual.
  const pfInitPath = path.join(prDir, '.pitchfork-init');
  await fs.writeFile(pfInitPath, JSON.stringify(params));
  return vscode.Uri.file(prDir);
}
