declare var _LABEL: string | undefined;
declare var _setGlobalConsole: (console: any) => void;
declare var nativeLoggingHook: (message: string, severity: number) => void;
declare var _log: (message: string) => void;
declare var performance: {
  now: () => number;
};
