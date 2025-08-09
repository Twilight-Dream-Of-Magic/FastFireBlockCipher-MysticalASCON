**中文：**

在轻量级密码学的舞台上，我一直是那个“直觉狂”——敢在非线性层上玩非置换结构，还能把扩散做得飞快。但狂不是乱来，狂也可以严谨。
FastFire 这个名字，不是为了耍帅，而是因为我希望它像点燃引信一样，瞬间把状态烧透——快、猛、无废招。

我尊重轻量级密码学的前辈 ASCON，它的非线性层让我血液沸腾——那种优雅的布尔逻辑，让我当场冒出了一串火花般的构想，并立刻把它用门电路缠绕实现成了这个“变态版”的 bit-sliced mapping box。它不追求 bijective S-box 的那种数学“正经”，而是完全奔着非线性与扩散的极限去堆——既让硬件跑得爽，又能在多轮置换里把差分和线性概率碾下去。

这份分析不是为了吹嘘，而是为了让一切有据可查——哪怕是靠直觉点燃的火，也得用实验和数据去证明它烧得够旺、够稳。

**English:**

In the arena of lightweight cryptography, I’ve always been the “intuition maverick” — bold enough to use a non-permutation structure for the nonlinear layer, yet still achieve blazing-fast diffusion. But bold doesn’t mean sloppy; being wild can be rigorous, too.
The name *FastFire* isn’t just for flair — I wanted it to ignite the state like a fuse, burning through instantly: fast, fierce, and without wasted moves.

I hold deep respect for the pioneers of lightweight cryptography, especially ASCON. Its nonlinear layer lit a fire in my mind — that elegant Boolean logic sparked an idea on the spot, which I immediately wove into this “extreme” bit-sliced mapping box using a gate-level design. It doesn’t chase the mathematical propriety of a bijective S-box; instead, it pushes nonlinearity and diffusion to the edge — keeping hardware-friendliness while crushing differential and linear probabilities over multiple permutation rounds.

This analysis isn’t about bragging — it’s about keeping things verifiable. Even a design born from a spark of intuition deserves to be backed by experiments and data proving that the fire burns hot and steady.


**English**
When I built **FastFire**, I didn’t start with academic papers or draw neat Boolean algebra diagrams. I built it on *feel*—the intuition of tangling logic gates together. **But** don’t mistake intuition for recklessness; intuition can be precise. I knew exactly what I wanted: not a permutation, not a balance for its own sake, but a *non-permutation nonlinear mix* that “ignites” instantly and spreads like wildfire. That’s why it’s called *FastFire*.
Later, I verified it with exhaustive enumeration and Monte-Carlo tests, and sure enough—it diffuses differences across the entire 17×64-bit state in just a few rounds, with linear biases kept well in check. **This isn’t luck; it’s calculated madness.**

# FastFire — Bit‑Sliced Mapping Box & Permutation: Empirical Analysis

**Scope**: This report measures the *non-permutation* bit‑sliced mapping box (`CustomBoxWithBitwise`, Steps 1–3–3.1) and runs Monte‑Carlo checks on the full **Permutation** (with the linear word‑mix plus the 17‑word rotation) for rounds r∈{1,2,4,9}.

---

## 1) Local 8→8 Bit‑Sliced Mapping Box (single‑bit lane)

This treats `(x0..x7)` as **one‑bit signals** (i.e., a single lane), producing a byte‑to‑byte mapping for all 256 inputs.

- Distinct outputs: **164 / 256**  → *not a permutation by design.*
- Bit balance (ones count over 256 inputs): **[160, 128, 128, 128, 96, 128, 128, 144]**  
  (ideal is 128 for each output bit; here bit0=160, bit4=96, bit7=144 are biased.)
- Max non‑trivial DDT entry: **128 / 256  (= 1/2)**  
  → there exist input differences Δ such that Pr[Δ → Δ′] = 1/2 through this box.
- Max non‑trivial LAT absolute sum: **128 / 256  (= 1/2)**  
  → there exist linear masks (a,b) with empirical correlation **±0.5** on this box.

### Strict Avalanche Criterion (SAC)
Probability that flipping input bit *i* flips output bit *j* (exact over all 256 inputs):

```
[['0.500', '0.500', '0.500', '0.500', '0.500', '0.500', '0.500', '0.500'], ['0.500', '0.500', '0.500', '0.000', '0.500', '0.500', '1.000', '0.500'], ['0.000', '1.000', '0.500', '0.500', '0.500', '0.500', '0.500', '0.000'], ['0.000', '1.000', '0.500', '0.500', '0.500', '0.500', '0.500', '0.500'], ['0.500', '0.500', '0.500', '0.500', '0.500', '0.500', '0.500', '0.500'], ['0.500', '0.500', '0.500', '0.000', '0.500', '0.500', '1.000', '0.500'], ['0.500', '0.500', '0.500', '0.500', '0.500', '0.500', '0.500', '0.500'], ['0.500', '1.000', '0.500', '0.500', '0.500', '0.500', '0.500', '0.500']]
```

Half of the entries sit at 0.5 (good), while several positions hit **0** or **1**, confirming the strong linear/differential structure within the local box (expected because NAND/NOR inject biased terms, then linear coupling moves—not removes—the bias).

> **Design note**: This “bit‑sliced mapping box” is **intentionally non‑bijective**, serving as a *nonlinear mixing* layer, not an S‑box permutation. (Terminology per author.)

---

## 2) Full Permutation (64‑bit words, 17‑word state)

**Setup**: random states S, single‑bit input difference injected in `State[0]` (rate word), bit position randomized each trial; r rounds; 4,000 trials.

### Differential diffusion (total Hamming weight of Δ after r rounds)

```
[{'rounds': 1, 'samples': 4000, 'avg_total_hw': 11.8635, 'min_total_hw': 3, 'max_total_hw': 24}, {'rounds': 2, 'samples': 4000, 'avg_total_hw': 80.4075, 'min_total_hw': 10, 'max_total_hw': 172}, {'rounds': 4, 'samples': 4000, 'avg_total_hw': 330.1865, 'min_total_hw': 230, 'max_total_hw': 394}, {'rounds': 9, 'samples': 4000, 'avg_total_hw': 493.1755, 'min_total_hw': 406, 'max_total_hw': 555}]
```

The average HW climbs rapidly and narrows, indicating fast diffusion across the 17 words by rounds 4–9.

### Linear correlation search (Monte‑Carlo)

Randomly sample 32 mask pairs (a over input, b over output), each estimated over 1,000 random states.

```
[{'rounds': 1, 'mask_trials': 32, 'samples_per_mask': 1000, 'max_abs_corr': 0.082}, {'rounds': 2, 'mask_trials': 32, 'samples_per_mask': 1000, 'max_abs_corr': 0.082}, {'rounds': 4, 'mask_trials': 32, 'samples_per_mask': 1000, 'max_abs_corr': 0.088}, {'rounds': 9, 'mask_trials': 32, 'samples_per_mask': 1000, 'max_abs_corr': 0.094}]
```

Observed max empirical correlation stays in the **~0.08–0.09** range after ≥1 rounds for these random masks, i.e., no obvious high‑bias linear relation leaked by the *full* permutation under this sampling (note: not an exhaustive search).

---

## 3) Mathematical reading (concise)

Let the local box implement F: {0,1}^8 → {0,1}^8 built from quadratic monomials (NAND/NOR → degree‑2 polynomials over GF(2)) and a final linear/XNOR mixing. Then:

1. **Non‑permutation**: rank deficiencies occur because outputs reuse intermediate signals with complements; we count **164** unique images over 256 inputs.
2. **Degree = 2**: algebraic normal forms of each output bit have degree 2 (no cubic or higher terms introduced by the box); overall nonlinearity is quadratic.
3. **DDT/LAT bounds**: The quadratic structure + re‑use of high‑degree hubs (x1,x4) yields max DDT **1/2** and max LAT correlation **1/2** at the local box. These are not fatal per se because the **permutation** subsequently applies strong word‑wise diffusion and state rotation; multi‑round composition multiplies the trail probabilities.
4. **Round‑wise effect**: the permutation’s linear layer is invertible and quickly spreads activity, so after a few rounds (empirically 4–9 here) single‑bit disturbances occupy a large fraction of the state. This reduces the feasibility of low‑weight differential/linear trails.

---

## 4) Takeaways for the README

- This component is a **bit‑sliced mapping box** (not a bijective S‑box). It injects *fast* nonlinearity via **NAND/NOR + XOR** with shallow gate depth and strong hardware affinity.
- Local (single‑lane) properties: distinct outputs **164/256**, max non‑trivial DDT **128/256**, max non‑trivial LAT abs **128/256**; several output bits exhibit bias (160/96/144 ones).
- System‑level (permutation) behavior: with **≥4 rounds**, diffusion across the 17×64‑bit state is fast (average Δ HW ~330 for r=4; ~493 for r=9 in our MC), and brute linear correlations found by a light MC search are ≤ **~0.09**.
- Security stance: the design *intentionally* avoids bijectivity at the local box; security relies on (i) repeated non‑linear injections, (ii) strong linear diffusion across words, and (iii) state rotation with round constants. A full cryptanalytic evaluation should include wide‑trail–style bounds for differential/linear trails across rounds; this report gives initial empirical evidence.

---

*Script provenance:* Generated to mirror the C++ as of the shared snippet. Re‑run to regenerate all numbers.


Nice—here’s a tight, **English-only math derivation** you can paste **right before** your “Empirical Analysis” heading. It explains the bit-sliced mapping box as a quadratic Boolean network, clarifies what **Step 3-1** is doing, and ties those to the observed stats once the **Permutation** is applied.

---

# Mathematical Derivation and Rationale (Concise)

This section formalizes the *bit-sliced mapping box* (the `CustomBoxWithBitwise` step) and its interaction with the full **Permutation**. We model all signals over the field **GF(2)** (bits with XOR as addition, AND as multiplication). The analysis explains (i) why the local box is non-bijective by design, (ii) why its local DDT/LAT extremes can reach 1/2, (iii) how **Step 3-1** acts as an affine, invertible post-mixing that helps redistribute polarity and local bias, and (iv) why multi-round composition with the linear diffusion and 17-word state rotation quickly suppresses exploitable trails.

## A. The bit-sliced mapping box is a quadratic Boolean network

Let the input to the box be $x=(x_0,\dots,x_7)\in\mathbb{F}_2^8$. The code performs:

1. **Step 1 (linear shuffle)** — an invertible linear transform

$$
x \leftarrow L_1 x,
$$

where $L_1$ is an $8\times 8$ binary matrix (each “$\hat{=}$” XOR assignment is a row operation). This step is linear, hence degree-preserving.

2. **Step 2 (nonlinear temps via NAND/NOR)**
   For inputs $a,b \in \mathbb{F}_2$:

$$
\mathrm{NAND}(a,b)=\neg(a\land b)=1+ab,\qquad
\mathrm{NOR}(a,b)=\neg(a\lor b)=\neg a \land \neg b=(1+a)(1+b)=1+a+b+ab.
$$

Thus each temporary $t_i$ is an **affine-quadratic** form in the current $x$’s: it contains a constant term, possibly linear terms (for NOR), and a quadratic monomial $ab$.

3. **Step 3 (XOR-inject temps back)**
   Outputs after this injection can be written as

$$
z \;=\; L_2 x \;\oplus\; q(x) \;\oplus\; c,
$$

where $L_2$ is linear over $\mathbb{F}_2$, $q(x)$ collects all degree-2 monomials coming from the NAND/NORs, and $c$ is a constant vector from the complements introduced in Step 2. Hence **each output bit has algebraic degree $2$**. The box is therefore a **quadratic Boolean network**, not a bijective S-box.

**Why non-permutation and why large local biases are expected.**
Because Step 2 injects **constants** and **quadratic reuse of hub variables** (notably the pairs that repeat across $t_i$), collisions are inevitable: distinct $x$ can map to the same $z$. Also, NOR contributes extra linear terms $a+b$, making certain outputs statistically biased unless the subsequent mixing cancels them. This explains observations like “164 distinct images out of 256”, and local DDT/LAT extremes reaching **$1/2$** on the single-lane (8→8) view—**by design**, because this component is a *non-permutation nonlinear mixing box*.

## B. Step 3-1 is an affine, invertible post-mixing

The code then runs:

```
x0 ^= x1;
x1 ^= ~x2;   # i.e., x1 ← x1 ⊕ (1 ⊕ x2)
x2 ^= x3;
x3 ^= x4;
x4 ^= x5;
x5 ^= x6;
x6 ^= ~x7;   # i.e., x6 ← x6 ⊕ (1 ⊕ x7)
x7 ^= x0;    # uses the updated x0
```

Algebraically, this is an **affine** map

$$
y \;=\; A\,z \;\oplus\; b,
$$

with $A\in \mathrm{GL}(8, \mathbb{F}_2)$ (invertible; the in-place XOR chain is a composition of elementary row operations) and a constant vector $b$ capturing the two complements. Step 3-1 therefore:

* **Preserves degree** (still degree 2 overall),
* **Redistributes polarity** (the two complements inject fixed offsets),
* **Stitches each output to at least two inputs** (via the XOR chain), reducing the chance that a single hub variable carries most of the weight into the next round, and
* **Prepares the words** for the following word-wise diffusion, so that each $t_i$’s influence is seen by multiple 64-bit lanes and multiple words on the next round.

Importantly, Step 3-1 **cannot** turn the local box into a permutation (and that’s not the goal). It’s there to *move* and *dilute* the local biases introduced by NAND/NOR before the heavy diffusion layer.

## C. Composition with the full Permutation

One permutation round consists of:

1. **Round constant** injection into word 8 (breaks symmetry),
2. **Bit-sliced mapping box** applied to words 0–7 **in parallel across 64 lanes**,
3. **Word-wise linear diffusion**: each 64-bit word is XORed with two of its rotations, and
4. **17-word cyclic rotation** of the state vector.

Let $S\in(\mathbb{F}_2^{64})^{17}$ be the state. Each round has the form

$$
S \;\mapsto\; \underbrace{L_{\text{glob}}}_{\text{invertible linear on 17 words}}
\Big(\underbrace{Q_{\text{local}}(S)}_{\text{degree 2 on 8 words}}\Big)\;\oplus\;\text{const},
$$

where $Q_{\text{local}}$ applies the quadratic box bit-sliced across lanes and $L_{\text{glob}}$ is the big linear mix (word XOR-rotations + 17-word rotate). Standard degree-growth reasoning for SPN-like constructions implies the **algebraic degree doubles at most per round**, so after $r$ rounds, each output bit has degree

$$
\deg \le \min(64,\; 2^r).
$$

In practice, the global diffusion rapidly grows the **number of active words/lanes**, so any differential or linear trail must cross many active quadratic boxes, making its probability/correlation **multiplicative** across rounds.

## D. Why the empirical numbers look the way they do

* **Local box (single lane, 8→8 view)**
  Quadratic + constants + hub reuse ⇒ **non-bijective** (164 distinct outputs) and **strong local relations** (max DDT/LAT **1/2**). SAC shows many entries at **0.5** (good mixing) and a few at **0 or 1** (expected from local structure and complements). This is intentional: the local box is a *non-permutation nonlinear mixer*, not a balanced bijective S-box.

* **After the Permutation**
  The **affine Step 3-1** first “whitens and stitches” the eight wires; the **word-wise rotation-XOR** then spreads each wire’s influence within its 64-bit word; the **17-word rotate** spreads disturbances across positions round-to-round. Empirically, a one-bit input difference reaches average total Hamming weight $\approx 330$ by **4 rounds** and $\approx 493$ by **9 rounds** (over 4000 trials), i.e., it lights up a large fraction of the 1088 state bits. A light Monte-Carlo linear search finds max correlations around **0.08–0.09** after ≥1 round on random masks—consistent with the expectation that multi-round diffusion forces trails to pick up many active quadratic boxes, driving probabilities/correlations down.

## E. Takeaway

* The **bit-sliced mapping box** is *not* a permutation by design; it is a **quadratic nonlinear mixer** built from **NAND/NOR + XOR**, hardware-friendly with shallow gate depth.
* **Step 3-1** is an **affine, invertible post-mix**: it redistributes polarity (two complements), stitches dependencies (XOR chain), and sets up the state for the global diffusion to do its job in the next round.
* The multi-round **Permutation** composes this nonlinear injection with strong linear diffusion and position rotation; degree grows, active words multiply, and differential/linear trails become multiplicatively more expensive.
* The observed stats (local DDT/LAT at 1/2; fast global diffusion; small empirical linear correlations after several rounds) align with this model.

*Bottom line:* **FastFire** earns its name: the local mixer “ignites” quickly, and the permutation fans that spark into a state-wide burn in a few rounds—*fast, fierce, and engineered to be verified.*

# Appendix — Why This Isn’t “Easy” (and why my “intuition” actually works)

This design may look simple on the surface—NAND/NOR + XOR, a short post-mix, then a word-wise diffusion and a 17-word rotate—but making it work **reliably** is not trivial. The bit-sliced mapping box is **intentionally non-bijective** and **quadratic**; the permutation depends on **multi-round composition** to crush exploitable trails. Below is a compact mathematical frame to show what you must control (and why I dared to be this “reckless” only after doing the homework).

## A. Minimal formalism you actually need

Let everything be over $\mathrm{GF}(2)$. For the local box $F:\{0,1\}^8\to\{0,1\}^8$:

* Each output component $F_j(x)$ has **algebraic normal form** (ANF)

  $$
  F_j(x)\;=\; c_j \;\oplus\; \sum_i \alpha_{j,i} x_i \;\oplus\; \sum_{i<k} \beta_{j,i,k}\, x_i x_k,
  $$

  hence **$\deg F_j=2$** (quadratic). NAND contributes $1+x_ix_k$; NOR contributes $1+x_i+x_k+x_ix_k$.
* **Differential distribution** (vectorial):

  $$
  \mathrm{DP}_F(a\!\to\!b)\;=\;2^{-8}\#\{x:\; F(x)\oplus F(x\oplus a)=b\}.
  $$

  Our local box attains $\max_a \max_b \mathrm{DP}_F(a\!\to\!b)=\tfrac12$ (empirically).
* **Linear correlation** (Walsh):

  $$
  \mathcal{W}_F(a,b)\;=\;\sum_{x\in\{0,1\}^8}(-1)^{\langle a,x\rangle \oplus \langle b,F(x)\rangle},\quad
  \mathrm{corr}(a,b)=\frac{\mathcal{W}_F(a,b)}{2^8}.
  $$

  We observe $\max_{a,b\neq 0}|\mathrm{corr}(a,b)|=\tfrac12$ locally.
* **Autocorrelation (per component)** for $g(x)=\langle b,F(x)\rangle$:

  $$
  \mathrm{AC}_g(u)=\sum_x (-1)^{g(x)\oplus g(x\oplus u)}.
  $$

  Step 2’s constants and hub reuse force strong values (0 or 1 in SAC entries), which Step 3-1 helps redistribute (it **cannot** remove them—only move/comb).

These are not “gotchas”—they are **deliberate**: the local box is a **non-permutation nonlinear mixer**, not a balanced S-box.

## B. What Step 3-1 actually does (and doesn’t)

Step 3-1 is an **affine, invertible** post-mix $y=A\,z\oplus b$ (two complements create the constant $b$). Consequences:

* **Degree preserving**: the overall degree stays 2 (good—cheap gates, strong enough).
* **Bias motion, not deletion**: it **re-phases** linear/bias terms so they don’t sit on the same wires across rounds.
* **Stitching**: the XOR chain couples each output to neighbors, which is exactly what the **next** word-wise diffusion can leverage.

It **does not** turn the box into a permutation (and isn’t meant to). It simply pre-conditions the signals for the heavy linear diffusion layer.

## C. Why multi-round composition helps (bounds you should have in mind)

One permutation round on the 17×64-bit state has the form

$$
S \;\mapsto\; L_{\text{glob}}\big(Q_{\text{local}}(S)\big)\oplus \text{const},
$$

where $Q_{\text{local}}$ applies 8 parallel **quadratic** mixers (bit-sliced across 64 lanes), and $L_{\text{glob}}$ is **invertible linear** (XOR with rotations per word) plus a **17-word rotate**.

* **Degree growth**: for SPN-like compositions with quadratic nonlinearity,

  $$
  \deg(\text{r rounds}) \;\le\; \min(64,\,2^r).
  $$

  Degree escalates quickly; linearizing a many-round cipher becomes costly.
* **Trail multiplication** (wide-trail mindset): if the local box has $\max\mathrm{DP}=\tfrac12$ and $\max|\mathrm{corr}|=\tfrac12$, then any $r$-round differential trail with $A_r$ **active boxes** is bounded by

  $$
  \mathrm{DP}_{\text{trail}}\;\le\;\Big(\tfrac12\Big)^{A_r},\qquad
  |\mathrm{corr}_{\text{trail}}|\;\le\;\Big(\tfrac12\Big)^{A_r}.
  $$

  The real game is lower-bounding $A_r$ using the linear layer’s mixing/branch behavior and the 17-word rotation. Our Monte-Carlo shows that even a **1-bit input difference** lights up **hundreds** of bits within a few rounds (avg total HW $\approx 330$ by 4 rounds, $\approx 493$ by 9), which is consistent with **large $A_r$** in practice.

## D. Why my “intuition” worked (and why you shouldn’t wing it)

My choices were **not** random:

* I knew **NAND/NOR** give **quadratic** terms with cheap gate depth, and I **accepted** local DDT/LAT = 1/2 as the *price* for a non-permutation mixer—**because** the global diffusion would multiply trails away across rounds.
* I used **hub reuse** (e.g., $x_1,x_4$) on purpose to **ignite** quickly, then **Step 3-1** to **move** those biases before diffusion—so the next round doesn’t keep hammering the same weak direction.
* The word rotations and the **17-word cyclic shift** are picked to **prevent partitioning** and to force disturbances to visit many coordinates, round after round.
* I verified the behavior with **full enumeration** on the local box and **Monte-Carlo** on the permutation. The numbers match the model.

If you don’t speak Boolean function theory, Walsh spectra, differential distributions, algebraic degree growth, and wide-trail reasoning—and if you don’t measure with code—you will almost certainly build something that “looks right” and **fails**.

## E. What makes cryptanalysis here nontrivial

* You must reason over **bit-sliced lanes** (64 in parallel), not just the 8-bit toy view.
* Useful attacks (differential/linear/integral/division-property/algebraic) depend on **how fast active words multiply**—that’s governed by the exact rotation constants and the cross-round word rotation. You don’t get that right by guessing.
* Because the local mixer is **non-bijective**, permutation-oriented tools (e.g., boomerang for S-boxes) don’t apply directly to the local box; you must lift your analysis to the **full permutation** where the mapping is invertible.

**Bottom line:** FastFire isn’t “easy”—it’s **fast** because the local mixer is brutal (quadratic, non-permutation) and because the permutation fans that spark across the whole state in a few rounds. That only works if you understand the algebra and you measure the system. Otherwise, you’ll copy the form and miss the substance.

