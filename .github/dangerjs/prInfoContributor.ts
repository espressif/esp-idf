import { DangerDSLType } from "danger";
declare const danger: DangerDSLType;

interface Contributor {
    login?: string;
}

const authorLogin = danger.github.pr.user.login;
const messageKnownContributor: string = `
***
👋 **Hi ${authorLogin}**, thank you for your another contribution to \`espressif/esp-idf\` project!

If the change is approved and passes the tests in our internal git repository, it will appear in this public Github repository on the next sync.
***
`;

const messageFirstContributor: string = `
***
👋 **Welcome ${authorLogin}**, thank you for your first contribution to \`espressif/esp-idf\` project!

📘 Please check [Contributions Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/contribute/index.html#contributions-guide) for the contribution checklist, information regarding code and documentation style, testing and other topics.

🖊️ Please also make sure you have **read and signed** the [Contributor License Agreement for espressif/esp-idf project](https://cla-assistant.io/espressif/esp-idf).

#### Pull request review and merge process you can expect
Espressif develops the ESP-IDF project in an internal repository (Gitlab). We do welcome contributions in the form of bug reports, feature requests and pull requests via this public GitHub repository.

1. An internal issue has been created for the PR, we assign it to the relevant engineer
2. They review the PR and either approve it or ask you for changes or clarifications
3. Once the Github PR is approved, we synchronize it into our internal git repository
4. In the internal git repository we do the final review, collect approvals from core owners and make sure all the automated tests are passing
    - At this point we may do some adjustments to the proposed change, or extend it by adding tests or documentation.
5. If the change is approved and passes the tests it is merged into the \`master\` branch
6. On next sync from the internal git repository merged change will appear in this public Github repository

***
`;

/**
 * Check whether the author of the pull request is known or a first-time contributor, and add a message to the PR with information about the review and merge process.
 */
export default async function (): Promise<string> {
    const contributors = await danger.github.api.repos.listContributors({
        owner: danger.github.thisPR.owner,
        repo: danger.github.thisPR.repo,
    });

    const contributorsData: Contributor[] = contributors.data;
    const knownContributors: (string | undefined)[] = contributorsData.map(
        (contributor: Contributor) => contributor.login
    );

    if (knownContributors.includes(authorLogin)) {
        return messageKnownContributor;
    } else {
        return messageFirstContributor;
    }
}
