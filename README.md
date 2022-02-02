<div align="center">
  <img src="img/icon.png" width="30%">
  <h1>react-native-multithreading</h2>
  <h3>🧵 Fast and easy multithreading for React Native using JSI. 🧵</h3>
  <h4>
    ⚠️ Note: This is a proof of concept. <br/>
    Do not use this library in production. <br/><br/>
    <a href="https://github.com/mrousavy/react-native-vision-camera">VisionCamera</a> goes beyond this concept and <br/>
    use a separate JS runtime to provide an API for processing <br/>
    camera frames in realtime. Check out <a href="https://mrousavy.com/react-native-vision-camera/docs/guides/frame-processors">Frame Processors</a>!
  </h4>
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

Since JSI is not officially released, installing **react-native-multithreading** requires you to edit a few native files. See [the setup guide (**SETUP.md**)](./SETUP.md) for more details.

> Requires react-native-reanimated [**2.1.0**](https://github.com/software-mansion/react-native-reanimated/releases/tag/2.1.0) or higher.

> 🎉 [v1.0](https://github.com/mrousavy/react-native-multithreading/releases/tag/1.0.0) with Android support is here! 🎉

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

Since `spawnThread` returns a `Promise`, you can also await the result. The React-JS Thread will not be blocked and will still be able to continue execution elsewhere (timers, callbacks, rendering, ...), while the custom thread runs the given function in a custom parallel runtime.

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

* You can run any JavaScript code you want in there.
* You can use variables from "outside" (e.g. state), but those will be immutable/frozen.
* You can use functions from "outside".
   - Worklets (functions with the `'worklet'` directive) can be called directly on the separate thread
   - Native JSI functions ("host functions", aka functions that print `function f() { [native code] }` when you call `.toString()` on them) can be called synchronously (e.g. functions from [react-native-mmkv](https://github.com/mrousavy/react-native-mmkv#usage))
   - Normal JS functions (e.g. setState) can be called on the React-JS thread with [`runOnJS`](https://docs.swmansion.com/react-native-reanimated/docs/api/runOnJS)
* You can assign Reanimated Shared Values.

> Note that react-native-multithreading is more of a proof of concept than a production ready library. Everything works as I listed it here, but in real world app you most likely won't be needing a JS multithreading library.

## What's not possible?

1. Since the library uses JSI for synchronous native methods access, remote debugging (e.g. with Chrome) is no longer possible. Instead, you should use [Flipper](https://fbflipper.com).
2. All functions you are calling inside a custom thread, must be workletized to truly run on a separate thread. So add the `'worklet'` directive at the top of every function you're calling in that thread (including the thread callback itself), and don't forget to install the Reanimated babel plugin.

## Supported JS engines

* JavaScript Core (JSC)
* [Hermes](http://hermesengine.dev)
* [V8](http://github.com/Kudo/react-native-v8) (iOS only)

## Performance

Since the worklets are completely dispatched in an isolated thread, nothing interrupts their execution. This means, the JS engine can optimize the functions really well, making execution fast.

Be aware that there always will be a small overhead when calling `spawnThread`, because all variables from outside have to be copied into the new thread first. For example, if you use the separate thread to do complex array operations, be aware that the array has to be copied into the separate thread first. Always benchmark the performance differences!

## Credits

* [react-native-reanimated](http://github.com/software-mansion/react-native-reanimated) for Shared Value adapting, essentially allowing JSI multithreading
* [**@karol-bisztyga**](https://github.com/karol-bisztyga) and [**@piaskowyk**](https://github.com/piaskowyk) for helping me understand a few parts of Reanimated better
* [**@Szymon20000**](https://github.com/Szymon20000) for taking time to review and merge my Reanimated PRs
* [JNI tips](https://developer.android.com/training/articles/perf-jni) and [fbjni](https://github.com/facebookincubator/fbjni) to make Android JNI interop easier
* [**@iamyellow**](https://github.com/iamyellow) for being a huge help on the Android side
* [Erik the Coder](https://www.erikthecoder.net/2019/03/30/async-does-not-imply-concurrent/) for the Icon
* You, for appreciating my work

> Note: Technically this is not [multithreading](https://en.wikipedia.org/wiki/Multithreading_(computer_architecture)), but rather [multiprocessing](https://en.wikipedia.org/wiki/Multiprocessing).
