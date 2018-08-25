/**
 * Module for extension settings, even those not stored in `settings.json`
 */ /** */

import * as vscode from 'vscode';

export interface GlobalSettings {
  baseDirPath?: string;
}

export interface LocalSettings {
  rootNamespace?: string;
}

export class SettingsAccess {
  constructor(public readonly extensionContext: vscode.ExtensionContext) {}

  get globalSettings(): GlobalSettings {
    return this.extensionContext.globalState.get<GlobalSettings>('settings', {});
  }

  async setGlobalSettings(s: GlobalSettings): Promise<void> {
    await this.extensionContext.globalState.update('settings', s);
  }

  get lobalSettings(): LocalSettings {
    return this.extensionContext.workspaceState.get<LocalSettings>('settings', {});
  }

  async setLocalSettings(s: LocalSettings): Promise<void> {
    return this.extensionContext.workspaceState.update('settings', s);
  }
}
