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

const fibonacci = (num: number): number => {
  'worklet';
  if (num <= 1) return 1;
  return fibonacci(num - 1) + fibonacci(num - 2);
};

export default function App() {
  const [isRunning, setIsRunning] = React.useState(false);
  const [input, setInput] = React.useState('');
  const [result, setResult] = React.useState<number | undefined>();

  const run = React.useCallback(async () => {
    setIsRunning(true);
    try {
      const parsedInput = Number.parseInt(input, 10);
      const fib = await spawnThread(() => {
        'worklet';
        return 7;
      });
      setResult(fib);
    } catch (e) {
      const msg = e instanceof Error ? e.message : JSON.stringify(e);
      Alert.alert('Error', msg);
    } finally {
      setIsRunning(false);
    }
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
