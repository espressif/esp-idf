class FatalError(RuntimeError):
    """
    Wrapper class for runtime errors that aren't caused by bugs in idf.py or the build process.
    """

    def __init__(self, message, ctx=None):
        super(RuntimeError, self).__init__(message)
        # if context is defined, check for the cleanup tasks
        if ctx is not None and 'cleanup' in ctx.meta:
            # cleans up the environment before failure
            ctx.meta['cleanup']()


class NoSerialPortFoundError(FatalError):
    pass
