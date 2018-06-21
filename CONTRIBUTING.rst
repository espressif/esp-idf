Contributions Guide
===================

We welcome contributions to the esp-idf project!

How to Contribute
-----------------

Contributions to esp-idf - fixing bugs, adding features, adding documentation - are welcome. We accept contributions via `Github Pull Requests <https://help.github.com/articles/about-pull-requests/>`_.

Before Contributing
-------------------

Before sending us a Pull Request, please consider this list of points:

* Is the contribution entirely your own work, or already licensed under an Apache License 2.0 compatible Open Source License? If not then we unfortunately cannot accept it.

* Does any new code conform to the esp-idf :doc:`Style Guide <style-guide>`?

* Does the code documentation follow requirements in :doc:`documenting-code`?

* Is the code adequately commented for people to understand how it is structured?

* Is there documentation or examples that go with code contributions? There are additional suggestions for writing good examples in :idf:`examples` readme.

* Are comments and documentation written in clear English, with no spelling or grammar errors?

* Example contributions are also welcome. Please check the :doc:`creating-examples` guide for these.

* If the contribution contains multiple commits, are they grouped together into logical changes (one major change per pull request)? Are any commits with names like "fixed typo" `squashed into previous commits <http://eli.thegreenplace.net/2014/02/19/squashing-github-pull-requests-into-a-single-commit/>`_?

* If you're unsure about any of these points, please open the Pull Request anyhow and then ask us for feedback.

Pull Request Process
--------------------

After you open the Pull Request, there will probably be some discussion in the comments field of the request itself.

Once the Pull Request is ready to merge, it will first be merged into our internal git system for in-house automated testing.

If this process passes, it will be merged onto the public github repository.

Legal Part
----------

Before a contribution can be accepted, you will need to sign our :doc:`contributor-agreement`. You will be prompted for this automatically as part of the Pull Request process.

Related Documents
-----------------

.. toctree::
    :maxdepth: 1

    style-guide
    documenting-code
    creating-examples
    ../api-reference/template
    contributor-agreement
