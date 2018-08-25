/**
 * Module to create directories
 */ /** */

import * as path from 'path';

import {BuildSystem, NewProjectParams} from './new';
import {fs} from './pr';

export function dirForNamespace(ns: string): string { return ns.replace(/::/g, '/'); }

export async function createDirectories(basePath: string, params: NewProjectParams) {
  const pfDir = path.join(basePath, params.name);
  // Create the directory where the project will live
  await fs.mkdir_p(pfDir);
  async function mkdir(subdir: string) { return fs.mkdir_p(path.join(pfDir, subdir)); }
  // Create the source directory
  const srcDir = path.join(pfDir, 'src');
  // And all the subdirectories based on the namespace
  await fs.mkdir_p(path.join(srcDir, dirForNamespace(params.rootNamespace)));
  // Generate the include/ dir, if applicable
  if (params.separateHeaders) {
    const incDir = path.join(pfDir, 'include');
    await fs.mkdir_p(path.join(incDir, dirForNamespace(params.rootNamespace)));
  }
  if (params.generateThirdParty) {
    await mkdir('third_party');
  }
  if (params.generateExamples) {
    await mkdir('examples');
  }
  await mkdir('tests');
  await mkdir('doc');
  if (params.buildSystem === BuildSystem.CMake) {
    await mkdir('cmake');
  }
}