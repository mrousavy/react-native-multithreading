import * as React from 'react';

import {
  StyleSheet,
  View,
  Text,
  TextInput,
  ActivityIndicator,
} from 'react-native';
import { spawnThread } from 'react-native-multithreading';

const fibonacci = (num: number): number => {
  if (num <= 1) return 1;
  return fibonacci(num - 1) + fibonacci(num - 2);
};

export default function App() {
  const [isRunning, setIsRunning] = React.useState(false);
  const [input, setInput] = React.useState('');
  const [result, setResult] = React.useState<number | undefined>();

  const run = React.useCallback(async () => {
    setIsRunning(true);
    const fib = await spawnThread(() => {
      const parsedInput = Number.parseInt(input, 10);
      const value = fibonacci(parsedInput);
      return value;
    });
    setResult(fib);
    setIsRunning(false);
  }, [input]);

  React.useEffect(() => {
    run();
  }, [run]);

  return (
    <View style={styles.container}>
      {isRunning ? (
        <ActivityIndicator />
      ) : (
        <>
          <Text>Input:</Text>
          <TextInput
            style={styles.input}
            value={input}
            onChangeText={setInput}
          />
          <Text>Fibonacci Number: {result}</Text>
        </>
      )}
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
  },
  input: {
    width: 60,
  },
});
