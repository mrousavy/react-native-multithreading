import 'react-native-gesture-handler';
import React from 'react';
import { AppRegistry } from 'react-native';
import Fibonacci from './src/Fibonacci';
import ThreadBlocking from './src/ThreadBlocking';
import { name as appName } from './app.json';
import { NavigationContainer } from '@react-navigation/native';
import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';

const Tab = createBottomTabNavigator();

function App() {
  return (
    <NavigationContainer>
      <Tab.Navigator>
        <Tab.Screen name="Fibonacci" component={Fibonacci} />
        <Tab.Screen name="Thread Blocking" component={ThreadBlocking} />
      </Tab.Navigator>
    </NavigationContainer>
  );
}

AppRegistry.registerComponent(appName, () => App);
