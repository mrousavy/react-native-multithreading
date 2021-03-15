import { NativeModules } from 'react-native';

type MultithreadingType = {
  multiply(a: number, b: number): Promise<number>;
};

const { Multithreading } = NativeModules;

export default Multithreading as MultithreadingType;
