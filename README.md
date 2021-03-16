<div align="center">
  <img src="img/icon.png" width="30%">
  <h1>react-native-multithreading</h2>
  <h3>🧵 Fast and easy multithreading for React Native using JSI.</h3>
  <br/>
  <a align="center" href='https://ko-fi.com/F1F8CLXG' target='_blank'>
    <img height='36' style='border:0px;height:36px;' src='https://az743702.vo.msecnd.net/cdn/kofi2.png?v=0' border='0' alt='Buy Me a Coffee at ko-fi.com' />
  </a>
  <br/>
  <a align="center" href="https://github.com/mrousavy?tab=followers">
    <img src="https://img.shields.io/github/followers/mrousavy?label=Follow%20%40mrousavy&style=social" />
  </a>
  <br/>
  <a align="center" href="https://twitter.com/mrousavy">
    <img src="https://img.shields.io/twitter/follow/mrousavy?label=Follow%20%40mrousavy&style=social" />
  </a>
</div>
<br/>

## Installation

```sh
npm install react-native-multithreading
npx pod-install
```

> Requires a version of react-native-reanimated which includes [PR #1790](https://github.com/software-mansion/react-native-reanimated/pull/1790). Either patch it yourself, or wait until that lands in a release.

> ⚠️ Warning: This is still just a proof of concept - do not use this library in production! ⚠️

## Why

Since [JSI](https://github.com/react-native-community/discussions-and-proposals/issues/91) is becoming more mainstream, there might be functions that are actually blocking and take a while to execute. For example, a storage library like [my react-native-mmkv](https://github.com/mrousavy/react-native-mmkv) or an SQLite JSI library might take a few milliseconds to execute a complex call. You don't want your entire React-JS thread to freeze when doing that, since users will perceive a noticeable lag or freeze.

That's where **react-native-multithreading** comes in; you can simply off-load such expensive calculations/blocking calls to a separate thread with almost no overhead while your main React-JS thread can concentrate on running your app's business logic, respond to user input, update state and more. You can also run complex JS calculations such as the [Fibonacci number](https://en.wikipedia.org/wiki/Fibonacci_number), but that's probably a rare use-case.

> Inspired by [**@karol-bisztyga**'s Multithreading PR for Reanimated](https://github.com/software-mansion/react-native-reanimated/pull/1561)

## Usage

To try out the Fibonacci Example, clone the repo and run the following commands:

```
yarn bootstrap
cd example
yarn ios
```

> See [my tweet 🐦](https://twitter.com/mrousavy/status/1371793888273432577)

### Shoot and Forget

To simply perform an expensive calculation on another thread without caring about the result, use the `spawnThread` function:

```ts
// JS thread
spawnThread(() => {
  'worklet'
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
  'worklet'
  // expensive calculation
  return ...
})
```

### Fibonacci

This example calculates the [Fibonacci Number](https://en.wikipedia.org/wiki/Fibonacci_number) for the given input. This demonstrates expensive calculation, awaiting the result, as well as using values from "outside". (`fibonacci` function and `input` are captured into the new thread and therefore immutable.)

```ts
const fibonacci = (num: number): number => {
  'worklet'
  if (num <= 1) return 1
  return fibonacci(num - 1) + fibonacci(num - 2)
}

const input = 50
const result = await spawnThread(() => {
  'worklet'
  console.log(`calculating fibonacci for input: ${input} in JS-Runtime: ${global._LABEL}...`)
  const fib = fibonacci(input)
  console.log("finished calculating fibonacci!")
  return fib
})
console.log(`Fibonacci Result: ${result}`)
```

## What's possible?

* You can use variables from "outside" (e.g. state), but those will be immutable/frozen.
* You can use functions from "outside" if they also contain the `'worklet'` directive.
* You can assign Reanimated Shared Values.
* You can call native JSI functions ("Host Functions") from a JSI library, e.g. every function [react-native-mmkv](https://github.com/mrousavy/react-native-mmkv#usage) provides.
* You can asynchronously dispatch calls to functions from "outside" using `runOnJS` from react-native-reanimated.

## What's not possible?

1. At the moment, only iOS is implemented. I cannot implement Android until react-native-reanimated gets published with source-code (no prebuilt .aar)
2. Since the library uses JSI for synchronous native methods access, remote debugging (e.g. with Chrome) is no longer possible. Instead, you should use [Flipper](https://fbflipper.com).
3. All functions you are calling inside a custom thread, must be workletized to truly run on a separate thread. So add the `'worklet'` directive at the top of every function you're calling in that thread (including the thread callback itself), and don't forget to install the Reanimated babel plugin.

## License

MIT

## Credits

* [react-native-reanimated](http://github.com/software-mansion/react-native-reanimated) for Shared Value adapting, essentially allowing JSI multithreading
* [Erik the Coder](https://www.erikthecoder.net/2019/03/30/async-does-not-imply-concurrent/) for the Icon
* You, for appreciating my work

> Note: Technically this is not [multithreading](https://en.wikipedia.org/wiki/Multithreading_(computer_architecture)), but rather [multiprocessing](https://en.wikipedia.org/wiki/Multiprocessing).
