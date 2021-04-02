import * as React from 'react';

import { StyleSheet, View, Text } from 'react-native';
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
          requestAnimationFrame(() => {
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
          });
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
      <Text style={styles.description}>
        In this example you can run a heavy benchmark/calculation on the default
        React-JS Thread and on the separate JS Thread. When the default React-JS
        Thread gets blocked, your entire application freezes - whereas the
        separate Thread does not affect application execution at all and is
        perfectly designed for heavy synchronous calculations.
      </Text>
      <PressableOpacity
        style={styles.aliveButton}
        onPress={() => console.log("REACT_THREAD: Hello, I'm alive 👋")}
      >
        <Text style={styles.buttonText}>
          Check if React-JS Thread is responsive
        </Text>
      </PressableOpacity>
      <PressableOpacity
        style={styles.button}
        onPress={() => runBenchmark('react')}
      >
        <Text style={styles.buttonText}>
          Run heavy calculation on React-JS Thread
        </Text>
      </PressableOpacity>
      <Text style={styles.isBlockedText}>
        React-JS Thread blocked: {isBlockingReactThread ? 'true' : 'false'}
      </Text>
      <PressableOpacity
        style={styles.button}
        onPress={() => runBenchmark('custom')}
      >
        <Text style={styles.buttonText}>
          Run heavy calculation on separate Thread
        </Text>
      </PressableOpacity>
      <Text style={styles.isBlockedText}>
        Custom Thread blocked: {isBlockingCustomThread ? 'true' : 'false'}
      </Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
    paddingTop: 50,
  },
  description: {
    maxWidth: '80%',
    fontSize: 15,
    color: '#242424',
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
  isBlockedText: {
    marginTop: 5,
    fontSize: 14,
    color: '#454545',
  },
  aliveButton: {
    marginTop: 10,
    marginBottom: 40,
    backgroundColor: '#90ee90',
    borderRadius: 10,
    paddingHorizontal: 15,
    paddingVertical: 10,
    maxWidth: '80%',
  },
  button: {
    marginTop: 10,
    backgroundColor: '#b35f5f',
    borderRadius: 10,
    paddingHorizontal: 15,
    paddingVertical: 10,
  },
  buttonText: {
    color: 'white',
    fontSize: 15,
    fontWeight: 'bold',
  },
});
