import * as React from 'react';

import { StyleSheet, View, Text, Button } from 'react-native';
import { spawnThread } from 'react-native-multithreading';
import 'react-native-reanimated';
import { PressableOpacity } from './PressableOpacity';

// complex branches with modular if so that it cannot be optimized very well. Takes ~4-5 seconds on my i9.
const benchmark = () => {
  'worklet';
  // global.performance is not yet installed. I will do that soon.
  const start = performance.now();
  function p(n: number) {
    for (var i = 2; i * i <= n; i++) {
      if (n % i === 0) {
        return false;
      }
    }
    return true;
  }

  var sum = 0;
  for (var k = 2; k < 1000000; k++) {
    if (p(k)) {
      sum++;
    }
  }
  const end = performance.now();
  return {
    result: sum,
    duration: end - start,
  };
};

const BENCHMARK_TIMES = 5;

export default function ThreadBlocking() {
  const [isBlockingCustomThread, setIsBenchmarkingCustom] = React.useState(
    false
  );
  const [isBlockingReactThread, setIsBenchmarkingReact] = React.useState(false);

  const runBenchmark = React.useCallback(
    async (thread: 'react' | 'custom' | 'ui') => {
      console.log(
        'Starting Benchmark - Please see native console (Xcode Logs/Android Logcat) for output!'
      );
      switch (thread) {
        case 'react': {
          setIsBenchmarkingReact(true);
          global.nativeLoggingHook(
            `REACT_THREAD: Begin blocking React-JS Thread...`,
            1
          );
          for (let i = 0; i < BENCHMARK_TIMES; i++) {
            const r = benchmark();
            global.nativeLoggingHook(
              `REACT_THREAD: Run #${i}: ${r.result} (took ${r.duration}ms)`,
              1
            );
          }
          global.nativeLoggingHook(
            `REACT_THREAD: React-JS Thread unblocked!`,
            1
          );
          setIsBenchmarkingReact(false);
          break;
        }
        case 'custom': {
          setIsBenchmarkingCustom(true);
          await spawnThread(() => {
            'worklet';
            _log(`CUSTOM_THREAD: Begin blocking Custom Thread...`);
            for (let i = 0; i < BENCHMARK_TIMES; i++) {
              const r = benchmark();
              // can't use console.log because that just dispatches to React-JS thread, which might be blocked.
              _log(
                `CUSTOM_THREAD: Run #${i}: ${r.result} (took ${r.duration}ms)`
              );
            }
            _log(`CUSTOM_THREAD: Custom Thread unblocked!`);
          });
          setIsBenchmarkingCustom(false);
          break;
        }
      }
    },
    []
  );

  return (
    <View style={styles.container}>
      <PressableOpacity
        onPress={() => console.log("REACT_THREAD: Hello, I'm alive 👋")}
      >
        <Text>Check if React-JS Thread is alive</Text>
      </PressableOpacity>
      <Button
        title="Run heavy calculation on React-JS Thread"
        onPress={() => runBenchmark('react')}
      />
      <Text>React-JS Thread blocked: {isBlockingReactThread}</Text>
      <Button
        title="Run heavy calculation on separate Thread"
        onPress={() => runBenchmark('custom')}
      />
      <Text>Custom Thread blocked: {isBlockingCustomThread}</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    marginTop: 150,
  },
  input: {
    width: '50%',
    paddingVertical: 5,
    marginVertical: 10,
    borderWidth: StyleSheet.hairlineWidth,
    borderRadius: 5,
    borderColor: 'black',
    textAlign: 'center',
    fontSize: 14,
  },
  bottom: {
    marginTop: 50,
  },
});
