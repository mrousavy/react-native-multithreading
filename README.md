<div align="center">
  <img src="img/icon.png" width="30%">
  <h1>react-native-multithreading</h2>
</div>

Fast and easy multithreading for React Native using JSI.

## Installation

```sh
npm install react-native-multithreading
npx pod-install
```

## How

By "adapting" a function and converting it to a "Shared Value", it was possible to decouple functions from the JS-Runtime (using Reanimated). The JSI library provides an easy to use API to create a new JSI Runtime Instance. Threads are re-used in a Thread-Pool, and Promises are used so results can be awaited.

## Usage

### Shoot and Forget

To simply do expensive calculation on another thread without caring about results, use the `spawnThread` function:

```ts
// JS thread
spawnThread(() => {
  // custom thread
  // expensive calculation
})
// JS thread
```

The React-JS Thread will continue execution while the custom thread will run the given function on a custom parallel runtime.

### Await

Since `spawnThread` returns a `Promise`, you can also await the result. The React-JS Thread will not be blocked and will still be able to continue execution elsewhere (timers, callbacks, ...), while the custom thread runs the given function in a custom parallel runtime.

```ts
const result = await spawnThread(() => {
  // expensive calculation
  return ...;
})
```

### Fibonacci

This example calculates the [Fibonacci Number](https://en.wikipedia.org/wiki/Fibonacci_number) for the given input. This demonstrates expensive calculation, awaiting the result, as well as using values from "outside". (`fibonacci` function and `input` are captured into the new thread and therefore immutable.)

```ts
const fibonacci = (num: number): number => {
  if (num <= 1) return 1
  return fibonacci(num - 1) + fibonacci(num - 2)
}

const input = 50
const result = await spawnThread(() => {
  console.log(`calculating fibonacci for input: ${input}...`)
  const fib = fibonacci(input)
  console.log("finished calculating fibonacci!")
  return fib
})
console.log(`Fibonacci Result: ${result}`)
```

## License

MIT

## Credits

* [react-native-reanimated](http://github.com/software-mansion/react-native-reanimated) for Shared Value adapting, essentially allowing JSI multithreading
* [Erik the Coder](https://www.erikthecoder.net/2019/03/30/async-does-not-imply-concurrent/) for the Icon

> Note: Technically this is not [multithreading](https://en.wikipedia.org/wiki/Multithreading_(computer_architecture)), but rather [multiprocessing](https://en.wikipedia.org/wiki/Multiprocessing).
