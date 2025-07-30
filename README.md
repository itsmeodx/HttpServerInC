


![C](https://img.shields.io/badge/language-C-blue.svg) ![HTTP](https://img.shields.io/badge/protocol-HTTP%2F1.1-green.svg) ![MIT](https://img.shields.io/badge/license-MIT-yellow.svg)

# 🚀 HTTP Server in C

Welcome! This is a fast, minimal, and modular HTTP/1.1 server written in C. It demonstrates:
- ⚡ Networking and socket programming
- 📁 File I/O and static file serving
- 📦 HTTP protocol parsing and response formatting
- 🏷️ MIME type detection (extension-based and fallback)
- 🧩 Modular design and robust error handling

**Source:** [src/](src/) | [inc/](inc/)

**Author:** [itsmeodx](https://github.com/itsmeodx)


## ✨ Features
- Serve static files from any directory
- RESTful endpoints:
  - `GET /echo/<text>` → 🗣️ Echoes text
  - `GET /user-agent` → 🕵️ Returns User-Agent header
  - `GET /files/<filename>` → 📥 Download files
  - `POST /files/<filename>` → 📤 Upload files
- Automatic MIME type detection for correct file serving


## ⚡ Quick Start
- Build and Run:
   ```sh
   ./your_program.sh
   ```

## Hints
- The server listens on port `4221` by default.
- The server serves files from the current directory.
- You can change the server's root directory by passing the `--directory` option:
   ```sh
   ./your_program.sh --directory /path/to/your/files
   ```
- For more options, run:
   ```sh
   ./your_program.sh --help
   ```

## 🌐 Example
Visit [`http://localhost:4221/echo/hello`](http://localhost:4221/echo/hello) in your browser.
Or try [`/files/index.html`](http://localhost:4221/files/index.html) for static file serving.


## 📁 Structure

```
.
├── src/
│   ├── main.c            # Entry point
│   ├── Http.c            # Core HTTP logic
│   ├── HttpEndPoints.c   # Endpoint handlers
│   ├── HttpMethods.c     # Routing logic
│   └── utils/            # Utility functions
├── inc/
│   └── Http.h            # Main header
```



## 📜 License
GNU General Public License v3.0

---
Made with ❤️ by [itsmeodx](https://github.com/itsmeodx)


<div align="center">
  <sub>Thanks for visiting! Happy coding 🚀</sub>
</div>

