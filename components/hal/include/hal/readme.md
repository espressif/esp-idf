# HAL Layer Readme

The HAL layer is designed to be used by the drivers. We don't guarantee the stability and back-compatibility among
versions. The HAL layer may update very frequently with the driver. Please don't use them in the applications or treat
them as stable APIs.

The HAL layer consists of two layers: HAL (upper) and Lowlevel(bottom). The HAL layer defines the steps and data
required by the peripheral. The lowlevel is a translation layer converting general conceptions to register configurations.

## Lowlevel

This layer should be all static inline. The first argument of LL functions is usually a pointer to the beginning address
of the peripheral register. Each chip should have its own LL layer. The functions in this layer should be atomic and
independent from each other so that the upper layer can change/perform one of the options/operation without touching the
others.

## HAL

This layer should depend on the operating system as little as possible. It's a wrapping of LL functions, so that the upper
layer can combine basic steps into different working ways (polling, non-polling, interrupt, etc.). Without using
queues/locks/delay/loop/etc., this layer can be easily port to other os or simulation systems.

To get better performance and better porting ability, ``context``s are used to hold sustainable data and pass the parameters.

To develop your own driver, it is suggested to copy the HAL layer to your own code and keep them until manual update.
