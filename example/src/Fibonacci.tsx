import * as React from 'react';

import {
  StyleSheet,
  View,
  Text,
  TextInput,
  ActivityIndicator,
  Alert,
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

export default function App() {
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
});
