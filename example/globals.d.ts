/**
 * Represents the current JS-Runtime's label.
 *
 * * Reanimated UI Runtime: `"UI"`
 * * Vision Camera Frame Processor: `"FRAME_PROCESSOR"`
 * * Multithreading Thread: `"CUSTOM_THREAD_N"` where `N` is the Thread's number
 * * React-JS Runtime: `undefined`
 */
declare var _LABEL: string | undefined;

/**
 * Sets the global console object
 */
declare var _setGlobalConsole: (console: any) => void;
