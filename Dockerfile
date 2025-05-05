FROM debian:bullseye

# Install build tools and testing utilities
RUN apt update && \
    apt install -y build-essential gcc make git netcat telnet vim libsdl2-dev texlive-latex-base texlive-latex-extra 

# Set working directory to your project folder
WORKDIR /secure-multiplayer-game-server

# Copy project files into the container
COPY . .

# Default shell when container starts
CMD ["bash"]


