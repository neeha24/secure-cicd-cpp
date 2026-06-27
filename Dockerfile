# ---------- Build stage ----------
# Compiles and runs the unit tests. If tests fail, the image never gets built.
FROM ubuntu:22.04 AS builder
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential cmake git ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build -j "$(nproc)" \
    && ctest --test-dir build --output-on-failure

# ---------- Runtime stage ----------
# Tiny final image: only the compiled binary, no compilers or build tools.
# Runs as a non-root user (a basic but important hardening practice).
FROM ubuntu:22.04 AS runtime
RUN useradd --create-home appuser
COPY --from=builder /app/build/sensor_stats /usr/local/bin/sensor_stats
USER appuser
ENTRYPOINT ["sensor_stats"]
