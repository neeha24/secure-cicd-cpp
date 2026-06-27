// Declarative Jenkins pipeline for the C++ app, with DevSecOps gates baked in.
//
// Assumes a LINUX agent that has: docker, cmake, g++, cppcheck, python3.
// All security tooling runs from official Docker images, so the agent really
// only needs Docker + a C++ toolchain. See README.md "Running in Jenkins".
pipeline {
    agent any

    options {
        timestamps()
        disableConcurrentBuilds()
    }

    environment {
        IMAGE_NAME = "sensor-stats"
        IMAGE_TAG  = "${env.BUILD_NUMBER}"
    }

    stages {
        stage('Checkout') {
            steps {
                checkout scm
                sh 'mkdir -p reports'
            }
        }

        stage('Build & Unit Test') {
            steps {
                sh '''
                    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
                    cmake --build build -j "$(nproc)"
                    ctest --test-dir build --output-on-failure
                '''
            }
        }

        stage('SAST - Static Analysis') {
            steps {
                // cppcheck is our open-source SAST tool. In a licensed shop this
                // stage would call Coverity or SonarQube instead — same idea.
                sh '''
                    cppcheck --enable=all --inconclusive --std=c++17 \
                        --xml --xml-version=2 src/ 2> reports/cppcheck.xml || true
                    cppcheck --enable=all --inconclusive --std=c++17 \
                        src/ 2> reports/cppcheck.txt || true
                '''
            }
        }

        stage('Secrets Scan') {
            steps {
                sh '''
                    docker run --rm -v "$PWD:/repo" \
                        zricethezav/gitleaks:latest detect \
                        --source=/repo --no-git \
                        --report-format=json \
                        --report-path=/repo/reports/gitleaks.json || true
                '''
            }
        }

        stage('Build Container Image') {
            steps {
                sh 'docker build -t $IMAGE_NAME:$IMAGE_TAG .'
            }
        }

        stage('SBOM Generation') {
            steps {
                // Software Bill of Materials in two standard formats.
                sh '''
                    docker run --rm \
                        -v /var/run/docker.sock:/var/run/docker.sock \
                        -v "$PWD/reports:/reports" \
                        anchore/syft:latest $IMAGE_NAME:$IMAGE_TAG \
                        -o cyclonedx-json=/reports/sbom.cyclonedx.json \
                        -o spdx-json=/reports/sbom.spdx.json
                '''
            }
        }

        stage('SCA - Image Vulnerability Scan') {
            steps {
                // Trivy scans the image's OS packages + dependencies. This is
                // the open-source stand-in for Black Duck.
                sh '''
                    docker run --rm \
                        -v /var/run/docker.sock:/var/run/docker.sock \
                        -v "$PWD/reports:/reports" \
                        aquasec/trivy:latest image \
                        --format json --output /reports/trivy.json \
                        --severity HIGH,CRITICAL \
                        $IMAGE_NAME:$IMAGE_TAG || true
                '''
            }
        }

        stage('Security Quality Gate') {
            steps {
                // Consolidate every scan into one report and FAIL the build if
                // findings cross our thresholds. This is the "shift-left" gate.
                sh 'python3 scripts/security_report.py reports/'
            }
        }
    }

    post {
        always {
            archiveArtifacts artifacts: 'reports/**', allowEmptyArchive: true
            archiveArtifacts artifacts: 'build/sensor_stats', allowEmptyArchive: true
        }
        failure {
            echo 'Pipeline failed — check the Security Quality Gate output above.'
        }
    }
}
