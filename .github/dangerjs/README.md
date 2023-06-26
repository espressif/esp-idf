# DangerJS pull request automatic review tool - GitHub

## Implementation
The main development is done in Espressif Gitlab project.
Espressif [GitHub project espressif/esp-idf](https://github.com/espressif/esp-idf) is only a public mirror. 

Therefore, all changes and updates to DangerJS files (`.github/dangerjs`) must be made via MR in the **Gitlab** repository by Espressif engineer. 

When adding a new Danger rule or updating existing one, might be a good idea to test it on the developer's fork of GitHub project. This way, the new feature can be tested using a GitHub action without concern of damaging Espressif's GitHub repository.

Danger for Espressif GitHub is implemented in TypeScript. This makes the code more readable and robust than plain JavaScript. 
Compilation to JavaScript code (using `tsc`) is not necessary; Danger handles TypeScript natively.

A good practice is to store each Danger rule in a separate module, and then import these modules into the main Danger file `.github/dangerjs/dangerfile.ts` (see how this is done for currently present modules when adding a new one).

If the Danger module (new check/rule) uses an external NPM module (e.g. `axios`), be sure to add this dependency to `.github/dangerjs/package.json` and also update `.github/dangerjs/package-lock.json`.

In the GitHub action, `danger` is not installed globally (nor are its dependencies) and the `npx` call is used to start the `danger` checks in CI.


## Adding new Danger rule
For local development you can use following strategy

#### Install dependencies
```sh
cd .github/dangerjs
npm install
```
(If the IDE still shows compiler/typing errors, reload the IDE window.)

#### Add new code as needed or make updates

#### Test locally
Danger rules can be tested locally (without running the GitHub action pipeline). 
To do this, you have to first export the ENV variables used by Danger in the local terminal:

```sh
export GITHUB_TOKEN='**************************************'
```

Then you can call Danger by:
```sh
cd .github/dangerjs

danger pr https://github.com/espressif/esp-idf/pull/<number_of_pull_request> 
```
The result will be displayed in your terminal.
