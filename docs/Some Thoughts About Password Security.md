# Part I

## Password Security: Easy Phrases vs. Complex Characters

Let's analyze the difficulty levels for hackers to crack passwords based on length and complexity.

## Password Cracking Methods

When attempting to crack passwords, hackers typically use techniques such as dictionary attacks, brute force attacks, and rainbow table attacks. Let's examine how these methods fare against different password types.

## Easy-to-Remember Words and Phrases

For passwords using common words and phrases:

**Short (1-8 characters)**
These are extremely vulnerable and can be cracked in seconds to minutes using dictionary attacks or brute force methods. Examples like "password" or "qwerty" offer virtually no security.

**Medium (9-16 characters)**
Simple phrases like "iloveyou123" can be cracked within hours to days. Dictionary-based passwords in this range provide only moderate protection against determined attackers.

**Longer (17-32 characters)**
Easy-to-remember longer phrases like "mychildrenarecalledtomandjane" significantly increase cracking difficulty. Even though they use dictionary words, the length alone pushes cracking time into weeks or months.

**Very Long (33-64 characters)**
Passphrases of this length, even if constructed from dictionary words, typically require years or decades to crack. The sheer length creates enormous computational requirements for attackers.

## Complex Mixed Characters

For passwords using mixed character types (uppercase, lowercase, numbers, symbols):

**Short (1-8 characters)**
Even with mixed characters, short passwords can be cracked relatively quickly—usually within days. While stronger than simple words, they remain vulnerable.

**Medium (9-16 characters)**
Complex passwords in this range (like "P@s$w0rd!2K24") significantly increase security, potentially requiring months to years to crack using brute force methods. This is because each additional character type exponentially increases the possible combinations.

**Longer (17-32 characters)**
Complex passwords of this length typically require decades to centuries to crack through brute force. Modern computing resources struggle with the computational complexity at this level.

**Very Long (33-64 characters)**
These passwords are practically uncrackable through brute force methods with current technology. The number of possible combinations reaches astronomical figures.

## Length vs. Complexity: What Matters More?

Research suggests that password length is actually more important than complexity. A longer, easy-to-remember passphrase (like "correct horse battery staple") can be more secure than a shorter complex password (like "P@s$w0rd!").

This is because each additional character in a password increases the time required to crack it exponentially, regardless of whether it's a simple letter or a special character.

## Best Practices Today

For maximum security with memorability:
- Use long passphrases (20+ characters) combining multiple random words
- Add minimal complexity (capitalize one letter, add one number/symbol)
- Use different passwords for different services
- Consider using a password manager for storage

The most important takeaway: A long, memorable passphrase provides better security than a short, complex, hard-to-remember password.


# Part II

## Password Cracking Time Algorithms by Security Level

Let's contemplate some specific algorithms used to calculate cracking times and consider the mathematical models behind each difficulty level assessment.

## Easy-to-Remember Words and Phrases

**Short (1-8 characters)**
The time calculation uses dictionary attack modeling:
- Average English dictionary contains ~170,000 common words
- Common password lists contain ~14 million entries
- Algorithm: T = N/R where T is time, N is number of attempts needed, R is attempt rate
- For 8-character common word passwords: T = (subset of common passwords)/10^9 attempts per second
- Example: "password" would be cracked almost instantly as it's in the top 10 of most password lists

**Medium (9-16 characters)**
Using combinatorial mathematics for common phrases:
- Algorithm: T = (W^P)/R where W is word pool size, P is phrase length
- For a 2-word phrase from 10,000 common words: T = (10^4)^2/10^9 = 10^8/10^9 = 0.1 seconds
- Adding numbers increases this modestly: T = (10^4)^2 × 10^3/10^9 = 0.1 × 10^3 = 100 seconds

**Longer (17-32 characters)**
Using entropy calculations:
- Algorithm: T = 2^E/R where E is entropy bits
- For 4-word passphrase from 10,000 word dictionary: E = log2(10^4)^4 = 53 bits
- T = 2^53/10^9 ≈ 9 × 10^6 seconds ≈ 104 days

**Very Long (33-64 characters)**
Using extended entropy calculations:
- Algorithm: T = 2^E/R where E is entropy bits
- For 8-word passphrase: E = log2(10^4)^8 = 106 bits
- T = 2^106/10^9 ≈ 8 × 10^22 seconds ≈ 2.5 × 10^15 years

## Complex Mixed Characters

**Short (1-8 characters)**
Using brute force algorithm:
- Algorithm: T = C^L/R where C is character set size, L is length
- For 8 chars with 95 possible characters: T = 95^8/10^9 ≈ 6.6 × 10^15/10^9 ≈ 6.6 × 10^6 seconds ≈ 76 days

**Medium (9-16 characters)**
Using extended brute force calculations:
- For 12 mixed characters: T = 95^12/10^9 ≈ 5.4 × 10^23/10^9 ≈ 5.4 × 10^14 seconds ≈ 17 million years
- With specialized hardware (10^12 attempts/sec): T ≈ 17,000 years

**Longer (17-32 characters)**
Using advanced cryptographic difficulty estimations:
- For 20 mixed characters: T = 95^20/10^12 ≈ 10^39/10^12 ≈ 10^27 seconds
- This exceeds the estimated age of the universe (4.3 × 10^17 seconds)

**Very Long (33-64 characters)**
Using astronomical difficulty calculations:
- For 40 mixed characters: T = 95^40/10^12
- The number of possible combinations exceeds the number of atoms in the observable universe

## Assumptions in These Calculations

1. Hardware capabilities: Baseline of 10^9 (1 billion) attempts per second for consumer hardware, up to 10^12 (1 trillion) for specialized cracking hardware

2. No shortcuts: Calculations assume no pattern exploitation, no reused passwords from data breaches

3. Perfect knowledge: Assumes attacker knows the exact password creation method but not the specific password

4. No quantum computing: Calculations use classical computing models; quantum computing would dramatically reduce these times for some encryption methods

These algorithms may demonstrate why password length provides more security benefit than complexity alone, as the time increases exponentially with each additional character, regardless of character type.



# Part III

## What does the future hold form quantum powered hackers?  Is it already too late?  What is the future of Quantum Computing and the Future of Password Security?  Are we worried about nothing?

The rise of quantum computing indeed presents significant challenges to our current cybersecurity paradigms. Let's explore what this means for password security and whether we're already at a critical point.

## The Quantum Threat to Passwords

Quantum computers pose a particular threat to cryptography through algorithms like Shor's algorithm and Grover's algorithm. The latter is especially relevant to password security:

While classical computers would require billions of years to break 256-bit encryption, quantum computers could potentially accomplish this in a matter of hours.

Specifically for password cracking:

- **Current passwords:** Quantum computers could potentially reduce the time to crack passwords exponentially. A complex 16-character password that would take centuries with classical computing might be vulnerable in days or hours with sufficient quantum power.

- **Encryption methods:** Many of today's encryption systems, particularly those based on RSA and ECC (Elliptic Curve Cryptography), are vulnerable to quantum attacks. These systems secure everything from our passwords to financial transactions.

## Are We Already Too Late?

The good news is that we're not yet at a "too late" stage, for several reasons:

1. **Current quantum capabilities:** Despite advances, current quantum computers still have limited qubit counts and high error rates. As of now, the most powerful quantum computers have under 1,000 qubits, whereas breaking common encryption would require millions of stable qubits.

2. **Quantum-resistant alternatives:** The cybersecurity community is actively developing post-quantum cryptography (PQC) algorithms designed to resist quantum attacks. In 2022, NIST selected several quantum-resistant cryptographic algorithms for standardization.

3. **Implementation timeline:** Experts estimate that quantum computers capable of breaking current encryption standards are still 5-10 years away from being developed, giving us time to implement new security measures.

## Preparing for the Quantum Future

To safeguard against future quantum threats:

1. **Post-quantum cryptography:** Organizations should begin planning for migration to quantum-resistant algorithms.

2. **Password strategies:** Even in a quantum computing era, extremely long passwords (50+ characters) will remain more resistant than shorter ones.

3. **Beyond passwords:** Multi-factor authentication that includes biometrics or physical tokens adds security layers that quantum computing can't easily bypass.

4. **Cryptographic agility:** Systems should be designed with "cryptographic agility" to quickly update encryption methods when vulnerabilities are discovered.



# Part IV

## We just aren't there yet

While quantum computing presents serious challenges to password security, we're not yet at a point of no return. The cybersecurity community is actively preparing defenses, and there's still time to implement quantum-resistant security measures. The real concern isn't whether we can respond, but whether organizations will implement these solutions quickly enough when more powerful quantum computers arrive. Now, let's explore some data-backed information about the current state of quantum computing and its implications for cryptography.

## Quantum Computing Capabilities: Current Status & Cryptography Implications

Based on the latest data, here's what we know about quantum computing capabilities and their implications for cryptography:

## Current Quantum Computing Capabilities (as of May 2025)

The world's largest quantum computers currently have between 1,100-1,200 qubits. IBM's Condor processor has 1,121 qubits, while Atom Computing has developed a system with 1,180 qubits using neutral atom technology.

There are approximately 100-200 quantum computers in existence globally, with the number steadily growing as technology improves. However, raw qubit count doesn't tell the full story.

The quality of qubits is actually more important than quantity. As experts note, "A quantum chip made up of thousands of low-quality qubits will be unable to perform any useful computational task." Current quantum computers are primarily being used by researchers to evaluate performance and develop improvements.

## The Critical Shift to Logical Qubits

A major breakthrough is the development of "logical qubits" - physical qubits connected through quantum entanglement that reduce errors by storing the same data in different places. QuEra has announced plans for a 30-logical-qubit machine with 3,000 physical qubits in 2025, followed by a 100-logical-qubit machine with over 10,000 physical qubits in 2026.

According to QuEra, "At 100 logical qubits, the [2026] machine can perform correct calculations that exceed the capability of today's supercomputers."  

Impressive or frightening?  Perhaps a <i>qubit<i> of both.

## Implications for Cryptography

The current state of quantum computing is not yet at the level where it threatens modern cryptography, but progress is accelerating:

Fujitsu and RIKEN have developed a 256-qubit quantum computer that will be available globally starting in early fiscal 2025. They're continuing R&D efforts toward a 1,000-qubit computer scheduled for installation in 2026.

IBM's roadmap aims for a 4,158+ qubit processor called "Kookaburra" by 2025.

## Timeline for Cryptographic Vulnerability

From these data points, we can make the following evidence-based conclusions:

1. Current quantum computers (1,000-1,200 qubits) are not yet capable of breaking modern cryptography.

2. The development of logical qubits is accelerating the timeline for quantum advantage.

3. The 100-logical-qubit systems projected for 2026 may represent the first machines capable of challenging certain cryptographic systems.

This aligns with my earlier assessment that we have approximately 5-10 years before quantum computers capable of breaking current encryption standards become available - enough time to implement quantum-resistant cryptography if organizations act promptly.

The evidence confirms we're not "too late," but the window for transition to post-quantum cryptography is narrowing faster than many expected.

## Conclusion

Doing business online is hard with a bunch of knotheads trying to crack your password. All we can do for now is try our best to make it difficult for them by utilizing password best practices.  By the time QC is a ubiquity, paradigms will have to shift.
