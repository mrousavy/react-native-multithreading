import React from 'react';
import {Button, SafeAreaView, useColorScheme, ViewStyle} from 'react-native';
import {Colors} from 'react-native/Libraries/NewAppScreen';
import RNMultiThreading from 'react-native-multithreading';

const App = () => {
  const isDarkMode = useColorScheme() === 'dark';

  const backgroundStyle: ViewStyle = {
    backgroundColor: isDarkMode ? Colors.darker : Colors.lighter,
    justifyContent: 'center',
    alignItems: 'center',
    flex: 1,
  };

  return (
    <SafeAreaView style={backgroundStyle}>
      <Button
        title="Spawn Thread"
        onPress={async () => {
          const fn = () => {
            'worklet';
            return true;
          };
          console.log(await RNMultiThreading.spawnThread(fn));
        }}
      />
    </SafeAreaView>
  );
};

export default App;
