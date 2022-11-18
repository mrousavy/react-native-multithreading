import { NativeModules } from 'react-native';
import { runOnJS } from 'react-native-reanimated';

const consoleInstaller = () => {
  const capturableConsole = console;
  return () => {
    'worklet';
    const console = {
      debug: runOnJS(capturableConsole.debug),
      log: runOnJS(capturableConsole.log),
      warn: runOnJS(capturableConsole.warn),
      error: runOnJS(capturableConsole.error),
      info: runOnJS(capturableConsole.info),
    };
    _setGlobalConsole(console);
  };
};

export const install = () => {
  //@ts-ignore
  if (global.spawnThread) return true;
  const installed = NativeModules.RNMultithreading.install();
  if (installed && global.spawnThread) {
    try {
      //@ts-ignore
      global.spawnThread(consoleInstaller());
    } catch (e) {
      console.log('react-native-multithreading:', (e as Error).stack);
    }
  }
  return installed;
};

/**
 * Runs the given function in a custom thread, in a custom parallel runtime.
 *
 * _Note: Despite it's name, Threads are actually not spawned on demand and will be re-used. (Thread-Pool)_
 *
 * @example
 * ```ts
 * const result = await spawnThread(() => {
 *   const someValue = doExpensiveCalculation()
 *   return someValue
 * })
 * ```
 */

declare global {
  var spawnThread: <T>(run: () => T) => Promise<T>;
  var doWork: <T>(code: string) => Promise<T>;
  var loadModuleFromAssets: (
    file: string,
    moduleName: string
  ) => Promise<boolean>;
  var loadPlugin: (pluginName: string) => Promise<boolean>;
}

interface RNMultithreadingInterface {
  spawnThread<T>(run: () => T): Promise<T>;
  doWork<T>(code: string): Promise<T>;
  loadModuleFromAssets(file: string, moduleName: string): Promise<boolean>;
  loadPlugin(pluginName: string): Promise<boolean>;
}

class RNMultithreadingWrapper implements RNMultithreadingInterface {
  spawnThread<T>(run: () => T): Promise<T> {
    install();
    return global.spawnThread(run);
  }
  doWork<T>(code: string): Promise<T> {
    install();
    return global.doWork(code);
  }
  loadModuleFromAssets(file: string, moduleName: string): Promise<boolean> {
    install();
    return global.loadModuleFromAssets(file, moduleName);
  }
  loadPlugin(pluginName: string): Promise<boolean> {
    install();
    return global.loadPlugin(pluginName);
  }
}

const RNMultithreading = new RNMultithreadingWrapper();

export default RNMultithreading;
