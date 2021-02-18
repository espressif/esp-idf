# No Embedded Paths

This test app exists to verify that paths (like __FILE__) are not compiled into
any object files in configurations where this should be avoided.

It doubles up as a build-time check that disabling assertions doesn't lead to
any warnings.

(These configurations include: assertions disabled, 'silent' asserts, any reproducible
builds configuration.)

Not embedding paths reduces the binary size, avoids leaking information about
the compilation environment, and is a necessary step to supporet reproducible
builds across projects built in different directories.
