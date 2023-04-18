/**
 * Check if PR has accurate description".
 *
 * @dangerjs WARN
 */

declare const danger: any;
declare const warn: any;

export default function (): void {
	const prDescription: string = danger.github.pr.body;
	const shortPrDescriptionThreshold: number = 100; // Description is considered too short below this number of characters

	if (prDescription.length < shortPrDescriptionThreshold) {
		return warn('The PR description looks very brief, please check if more details can be added.');
	}
}
