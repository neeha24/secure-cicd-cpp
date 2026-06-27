# Interview Guide

Everything here is for *you*, not for the repo. Read it a couple of times
before the interview.

---

## 1. The honest gap analysis

Your resume is strong on infrastructure, Linux, automation, and security
hardening. Against THIS job description, here are the gaps the DevOps engineer
will likely probe — and how this project closes (or addresses) each:

| Gap in your resume vs the JD | Severity | How to handle it |
|---|---|---|
| **Jenkins** (resume says "CI/CD" but never Jenkins) | HIGH | The whole project IS a Jenkins pipeline. Build it, run it, know every stage. |
| **C++ / CMake / embedded build systems** | HIGH | The project builds a real C++ app with CMake + GoogleTest. You don't need to be a C++ developer — you need to support C++ teams. That's exactly what this shows. |
| **Named SAST/SCA tools** (Coverity, Black Duck, SonarQube) | MEDIUM | You have the *concepts* (cppcheck, trivy, syft). Use the "open-source → enterprise" table from the README. Never pretend you've used Coverity if you haven't. |
| **Artifactory** | MEDIUM | You have JFrog's role-equivalent (archiving artifacts/SBOMs in Jenkins). Say you understand it as the artifact/binary repository and would use it for storage, promotion, and as a proxy for OSS packages. |
| **SBOM / supply chain security** | MEDIUM | The syft stage. Be able to explain what an SBOM is in one sentence (below). |
| **C++ debugging/compilers (gdb, clang)** | LOW | Mention you'd support the toolchain; you don't need deep C++ debugging. |
| **Vagrant / Confluence** | LOW | You have Docker/VMware and Jira. Easy adjacents — name them as "same category." |

The single most important reframe: **this role is not asking you to write
embedded C++. It's asking you to build and run the infrastructure that C++ and
firmware teams depend on.** That is squarely your background — you just need to
prove you can do it for *their* stack. This project is that proof.

---

## 2. Build it before the interview

Don't just read the code — actually run it end to end at least once, ideally
twice. The DevOps engineer will be able to tell within two questions whether you
built it or copied it. Time budget:

- **Tight (1–2 evenings):** Get `./scripts/build.sh` + Docker build + the Python
  report working locally (no Jenkins). Screenshot the output. You can demo from
  the terminal and the code.
- **Comfortable (a weekend):** Add Jenkins running in Docker and get the full
  pipeline green, then deliberately introduce the fake secret / a CRITICAL CVE
  base image to show the gate FAILING. A failing gate is a better demo than a
  passing one — it proves the security actually does something.
- **Stretch:** Push to Bitbucket, wire "Pipeline from SCM," spin up SonarQube
  via docker-compose and scan the Python script.

---

## 3. The 5-minute live demo script

If they let you screen-share, walk this arc. If not, walk the same arc through
the code + a screenshot.

1. **Frame it (15s):** "I saw the JD emphasised Jenkins, C++ builds, and
   DevSecOps, so I built a small pipeline that does all three. The app is just a
   C++ utility — the interesting part is the pipeline around it."
2. **Show the build + tests (45s):** CMake builds the library + CLI; GoogleTest
   runs under CTest; tests gate everything downstream.
3. **Show the Dockerfile (45s):** multi-stage — tests run in the build stage, the
   runtime image is slim and runs as a non-root user.
4. **Walk the Jenkinsfile stage by stage (2m):** build → SAST → secrets → image
   → SBOM → SCA → quality gate. For each, say what tool and what it catches.
5. **Show the gate failing (1m):** point at the fake secret / a CVE, run the
   Python report, show exit code 1 stopping the build. Then remove it, re-run,
   green. **This is your money shot.**
6. **Close (15s):** "In your environment cppcheck becomes Coverity, trivy
   becomes Black Duck, and artifacts land in Artifactory — same pipeline, same
   gates, different tools."

---

## 4. One-sentence definitions to have ready

- **SAST:** analyses source code for bugs/vulnerabilities without running it.
- **SCA:** finds known CVEs in your dependencies and base images.
- **SBOM:** a machine-readable inventory of everything in your build, so when
  the next Log4Shell hits you can answer "are we affected?" in minutes.
- **Shift-left:** move security checks as early in the pipeline as possible so
  problems are caught at commit time, not in production.
- **Quality gate:** automated pass/fail thresholds that stop a bad build from
  progressing.

---

## 5. Likely questions + strong answers

**"Walk me through what happens when a developer pushes code."**
> Bitbucket triggers the Jenkins pipeline. It checks out, builds with CMake and
> runs unit tests — if those fail it stops. Then SAST (cppcheck), a secrets
> scan, the container build, SBOM generation, and an image vulnerability scan.
> A Python script consolidates all findings and fails the build if anything
> crosses our thresholds. Reports get archived on the build.

**"You've used cppcheck, not Coverity. Is that a problem?"**
> The tool differs, the workflow doesn't. cppcheck sits in the SAST stage
> exactly where Coverity would, with the same job: block the build on findings
> over threshold. I'd be productive with Coverity quickly because I understand
> what that stage is *for*.

**"Why a multi-stage Docker build?"**
> So the final image only contains the compiled binary — no compilers, no build
> tools — which shrinks it and reduces attack surface. I also run the tests in
> the build stage so a broken build never produces an image.

**"How would you handle embedded targets / hardware-in-the-loop testing?"**
> Be honest: "I haven't done hardware-in-the-loop directly, but the pattern is
> the same — add a stage that flashes the firmware to a target or emulator and
> runs integration tests, gated like the others. I'd lean on the embedded team
> for the target-specific bits and own the pipeline and infrastructure around
> it." (Don't bluff hardware specifics.)

**"What's an SBOM and why care?"**
> Use the one-sentence definition above; mention CycloneDX and SPDX are the two
> standard formats and syft emits both.

**"How do you secure the software supply chain?"**
> Pin/lock dependencies, scan them (SCA), generate an SBOM, scan the built
> image, scan for secrets, and ideally use a trusted internal proxy
> (Artifactory) so you're not pulling straight from public registries.

**"How would you store and promote build artifacts?"**
> That's Artifactory's job — a binary repository for versioned artifacts, with
> promotion between repos (dev → staging → release) and as a caching proxy for
> public packages. Here I archive them in Jenkins as a stand-in.

---

## 6. Things NOT to do

- Don't claim hands-on with Coverity, Black Duck, or Artifactory if you haven't
  touched them. Senior interviewers smell it instantly, and one caught
  exaggeration poisons everything else you said.
- Don't pretend to be a C++ developer. You're the platform engineer who makes
  C++ teams productive. Own that lane.
- Don't memorise answers word-for-word — understand the *why* so you can handle
  follow-ups.

---

## 7. Questions to ask THEM (always have 3–4)

- What does your current CI/CD stack look like, and what's the biggest pain
  point you're hoping this role fixes?
- Are the embedded teams doing hardware-in-the-loop testing today, or is that
  something you want to build out?
- Which security tools are already in place (Coverity, Black Duck), and how
  mature is the DevSecOps adoption across teams?
- What does success in this role look like 6 months in?

Asking sharp questions about *their* pipeline signals you think like an owner,
not just a task-doer.
