# Secure CI/CD Pipeline for a C++ Application

A small but complete **DevSecOps pipeline** built around a tiny C++ "sensor
reading processor." The app is deliberately trivial — the point is everything
*around* it: the build, the tests, the container, and the security gates.

This project was built to demonstrate, in one place, the skills in the job
description: Jenkins CI/CD, Docker, C++/CMake builds, and DevSecOps (SAST, SCA,
SBOM, secrets scanning) with a security quality gate.

---

## The pipeline at a glance

```
  git push
     │
     ▼
┌─────────────────────────────────────────────────────────────┐
│ Jenkins declarative pipeline (Jenkinsfile)                  │
│                                                             │
│  Checkout                                                   │
│     │                                                       │
│  Build & Unit Test ........ CMake + GoogleTest (CTest)      │
│     │                                                       │
│  SAST ..................... cppcheck   (static analysis)    │
│     │                                                       │
│  Secrets Scan ............. gitleaks                        │
│     │                                                       │
│  Build Container Image .... multi-stage Dockerfile          │
│     │                                                       │
│  SBOM Generation .......... syft (CycloneDX + SPDX)         │
│     │                                                       │
│  SCA / Image Scan ......... trivy (HIGH/CRITICAL CVEs)      │
│     │                                                       │
│  Security Quality Gate .... security_report.py (Python)     │
│         └── fails the build if thresholds are exceeded      │
└─────────────────────────────────────────────────────────────┘
```

---

## Repo layout

```
.
├── src/                  C++ source (library + CLI)
├── tests/                GoogleTest unit tests
├── scripts/
│   ├── build.sh          Build+test on Linux/macOS
│   ├── build.ps1         Build+test on Windows (PowerShell)
│   └── security_report.py  Consolidates scans + quality gate (Python)
├── examples/             Intentional fake secret for the secrets-scan demo
├── CMakeLists.txt        Modern CMake (FetchContent pulls GoogleTest)
├── Dockerfile            Multi-stage: build+test, then slim non-root runtime
├── Jenkinsfile           The pipeline
├── docker-compose.yml    Optional local SonarQube (Community)
└── README.md             This file
```

---

## Prerequisites

To run the **full** pipeline you need: Docker, CMake (3.16+), a C++17 compiler,
cppcheck, and Python 3. The security tools (syft, trivy, gitleaks) run from
their official Docker images, so you don't have to install them separately.

To just build and run the **app** locally you only need CMake + a compiler.

---

## Run it locally (5 minutes, no Jenkins)

```bash
# 1. Build + test (needs internet first time, to fetch GoogleTest)
./scripts/build.sh                 # or  pwsh ./scripts/build.ps1 on Windows

# 2. Run the app
./build/sensor_stats 21.4 22.1 19.8 25.0
# -> count=4 min=19.8 max=25 mean=22.075

# 3. Build the container
docker build -t sensor-stats:dev .

# 4. Generate an SBOM
docker run --rm -v /var/run/docker.sock:/var/run/docker.sock \
  -v "$PWD/reports:/reports" anchore/syft:latest sensor-stats:dev \
  -o cyclonedx-json=/reports/sbom.cyclonedx.json

# 5. Scan the image
docker run --rm -v /var/run/docker.sock:/var/run/docker.sock \
  -v "$PWD/reports:/reports" aquasec/trivy:latest image \
  --format json --output /reports/trivy.json sensor-stats:dev

# 6. Static analysis + secrets scan
cppcheck --enable=all --xml --xml-version=2 src/ 2> reports/cppcheck.xml
docker run --rm -v "$PWD:/repo" zricethezav/gitleaks:latest detect \
  --source=/repo --no-git --report-format=json \
  --report-path=/repo/reports/gitleaks.json

# 7. Consolidated report + quality gate
python3 scripts/security_report.py reports/
```

---

## Run it in Jenkins

1. Run Jenkins itself in Docker (simplest):
   ```bash
   docker run -d --name jenkins -p 8080:8080 \
     -v /var/run/docker.sock:/var/run/docker.sock \
     -v jenkins_home:/var/jenkins_home jenkins/jenkins:lts
   ```
   (Mounting the Docker socket lets the pipeline build images. Install the
   Docker CLI, CMake, g++, cppcheck and python3 on the agent, or use a custom
   agent image that already has them.)

2. Push this repo to Bitbucket or GitHub.
3. New Item → **Pipeline** → "Pipeline script from SCM" → point it at your repo.
   Jenkins reads the `Jenkinsfile` automatically.
4. Build. Watch each stage run; the archived `reports/` show up on the build page.

---

## How this maps to the job description

| Job requirement | Where it shows up here |
|---|---|
| CI/CD pipelines (Jenkins) | `Jenkinsfile` — full declarative pipeline |
| Windows + Linux support | `build.ps1` + `build.sh`; CMake is cross-platform |
| Python / Bash / PowerShell | `security_report.py`, `build.sh`, `build.ps1` |
| DevSecOps integrated into CI | SAST + SCA + secrets + SBOM stages in the pipeline |
| SAST | cppcheck stage (→ Coverity/SonarQube in production) |
| SCA | trivy image scan (→ Black Duck in production) |
| SBOM generation | syft stage (CycloneDX + SPDX) |
| Software supply chain security | SBOM + image scan + secrets scan together |
| C++ / CMake / build systems | `src/`, `CMakeLists.txt`, GoogleTest |
| Containerized builds | multi-stage `Dockerfile` |
| Automated testing infra | CTest in CI; tests gate the image build |
| Quality gate / shift-left | `security_report.py` exits non-zero → fails build |
| Source control / Bitbucket | "Pipeline from SCM" wiring |
| Internal engineering utilities | the Python consolidation script |

---

## Open-source tool → enterprise tool

You almost certainly can't run Coverity or Black Duck at home (commercial
licenses). That's fine. **The concepts and pipeline placement are identical** —
you swap the tool, not the workflow. Be ready to say exactly this:

| Concept | Used here (free) | Enterprise equivalent (in the JD) |
|---|---|---|
| SAST (static analysis) | cppcheck, clang-tidy | Coverity, SonarQube |
| SCA (dependency CVEs) | trivy, grype | Black Duck |
| SBOM | syft | Black Duck, FOSSA |
| Secrets scanning | gitleaks | GitGuardian, native scanners |
| Artifact storage | archived in Jenkins | Artifactory |

---


