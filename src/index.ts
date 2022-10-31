import { NativeModules } from 'react-native';
import { runOnJS } from 'react-native-reanimated';

const g = global as any;

export const install = () => {
  if (g.spawnThread) return true;
  const installed = NativeModules.RNMultithreading.install();
  if (installed && g.spawnThread) {
    const capturableConsole = console;
    try {
      g.spawnThread(() => {
        'worklet';
        const console = {
          debug: runOnJS(capturableConsole.debug),
          log: runOnJS(capturableConsole.log),
          warn: runOnJS(capturableConsole.warn),
          error: runOnJS(capturableConsole.error),
          info: runOnJS(capturableConsole.info),
        };
        _setGlobalConsole(console);
      });
    } catch (e) {
      //@ts-ignore
      console.error('react-native-multithreading:', e.stack);
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



export const {
  spawnThread,
  doWork,
  loadModuleFromAssets,
  loadPlugin,
}: {
  spawnThread: <T>(run: () => T) => Promise<T>;
  doWork: <T>(code: string) => Promise<T>;
  loadModuleFromAssets: (file: string, moduleName: string) => Promise<boolean>;
  loadPlugin: (pluginName: string) => Promise<boolean>;
} = g;
