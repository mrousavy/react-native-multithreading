import * as React from 'react';

import {
  StyleSheet,
  View,
  Text,
  TextInput,
  ActivityIndicator,
  Alert,
  Button,
} from 'react-native';
import { spawnThread } from 'react-native-multithreading';
import 'react-native-reanimated';

// calculates the fibonacci number - that can be optimized really good so it's really really fast.
const fibonacci = (num: number): number => {
  'worklet';
  // Uses array to store every single fibonacci number
  var i;
  let fib: number[] = [];

  fib[0] = 0;
  fib[1] = 1;
  for (i = 2; i <= num; i++) {
    fib[i] = fib[i - 2] + fib[i - 1];
  }
  return fib[fib.length - 1];
};

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

export default function App() {
  const [isBenchmarkingCustom, setIsBenchmarkingCustom] = React.useState(false);
  const [isBenchmarkingReact, setIsBenchmarkingReact] = React.useState(false);
  const [isRunning, setIsRunning] = React.useState(false);
  const [input, setInput] = React.useState('5');
  const [result, setResult] = React.useState<number | undefined>();

  const runFibonacci = React.useCallback(async (parsedInput: number) => {
    setIsRunning(true);
    try {
      const fib = await spawnThread(() => {
        'worklet';
        console.log(
          `${global._LABEL}: Calculating fibonacci for input ${parsedInput}...`
        );
        const value = fibonacci(parsedInput);
        console.log(
          `${global._LABEL}: Fibonacci number for ${parsedInput} is ${value}!`
        );
        return value;
      });
      setResult(fib);
    } catch (e) {
      const msg = e instanceof Error ? e.message : JSON.stringify(e);
      Alert.alert('Error', msg);
    } finally {
      setIsRunning(false);
    }
  }, []);

  React.useEffect(() => {
    const parsedInput = Number.parseInt(input, 10);
    runFibonacci(parsedInput);
  }, [runFibonacci, input]);

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
      <Text>Input:</Text>
      <TextInput
        style={styles.input}
        value={input}
        onChangeText={setInput}
        placeholder="0"
      />
      {isRunning ? (
        <ActivityIndicator />
      ) : (
        <Text>Fibonacci Number: {result}</Text>
      )}

      <View style={styles.bottom}>
        <Button
          title="Run heavy calculation on React-JS Thread"
          onPress={() => runBenchmark('react')}
        />
        {isBenchmarkingReact && <ActivityIndicator />}
        <Button
          title="Run heavy calculation on separate Thread"
          onPress={() => runBenchmark('custom')}
        />
        {isBenchmarkingCustom && <ActivityIndicator />}
      </View>
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
