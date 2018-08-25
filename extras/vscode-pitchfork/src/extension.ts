'use strict';
import * as vscode from 'vscode';
import * as path from 'path';
import {SettingsAccess} from './settings';
import {getNewProjectParametersFromUI, createNewProject, NewProjectParams} from './new';
import { fs } from './pr';


class Extension {
  constructor(public readonly extensionContext: vscode.ExtensionContext) {}

  readonly settings = new SettingsAccess(this.extensionContext);

  get baseDirPath(): string|undefined { return this.settings.globalSettings.baseDirPath; }

  async changeBaseDir() {
    const old = this.baseDirPath;
    const chosen = await vscode.window.showOpenDialog({
      canSelectFiles: false,
      canSelectFolders: true,
      canSelectMany: false,
      defaultUri: old ? vscode.Uri.file(old) : undefined,
      openLabel: 'Select',
    });
    if (chosen === undefined) {
      return;
    }
    console.assert(chosen.length === 1, 'More than one file chosen?');
    const newSettings = {...this.settings.globalSettings, baseDirPath: chosen[0].fsPath};
    await this.settings.setGlobalSettings(newSettings);
  }

  async newProject() {
    let baseDir = this.baseDirPath;
    while (!baseDir) {
      const okayString = 'Okay';
      const chosen = await vscode.window.showInformationMessage(
          'Before creating a project, you must set the base directory for all projects',
          {modal: true},
          okayString,
      );
      if (chosen === undefined) {
        return;
      }
      console.assert(chosen === okayString, 'Clicked on other button?', chosen);
      await this.changeBaseDir();
      baseDir = this.baseDirPath;
    }

    const params = await getNewProjectParametersFromUI(baseDir);
    if (!params) {
      return;
    }
    const uri = await createNewProject(baseDir, params);
    vscode.commands.executeCommand('vscode.openFolder', uri, true);
  }
}

export async function activate(context: vscode.ExtensionContext) {
  const ext = new Extension(context);
  context.subscriptions.push(
      vscode.commands.registerCommand(
          'pf.changeBaseDir',
          async () => ext.changeBaseDir(),
          ),
      vscode.commands.registerCommand(
          'pf.newProject',
          async () => ext.newProject(),
          ),
  );
  if (vscode.workspace.workspaceFolders) {
    const first = vscode.workspace.workspaceFolders[0];
    const pfInitPath = path.join(first.uri.fsPath, '.pitchfork-init');
    const stat = await fs.tryStat(pfInitPath);
    if (stat && stat.isFile()) {
      try {
        const data: NewProjectParams = JSON.parse((await fs.readFile(pfInitPath)).toString());
        await ext.settings.setLocalSettings({
          rootNamespace: data.rootNamespace,
        });
        // Remove the file now that we have loaded its contents
        await fs.unlink(pfInitPath);
      } catch (e) {
        vscode.window.showErrorMessage('The .pitchfork-init file in this directory is not valid JSON.');
      }
    }
  }
}

// this method is called when your extension is deactivated
export function deactivate() {}