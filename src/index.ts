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
