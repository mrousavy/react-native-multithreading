import 'react-native-reanimated';
import { runOnJS } from 'react-native-reanimated';

const g = global as any;

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
export const spawnThread = g.spawnThread as <T>(run: () => T) => Promise<T>;

// TODO: Find a way to automatically bind console once I can spawn multiple threads. Possibly through a member function: Thread.polyfillConsole()
const capturableConsole = console;
spawnThread(() => {
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
