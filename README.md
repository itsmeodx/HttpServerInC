![C](https://img.shields.io/badge/language-C-blue.svg) ![HTTP](https://img.shields.io/badge/protocol-HTTP%2F1.1-green.svg) ![GNU GPL v3.0](https://img.shields.io/badge/license-GNU%20GPL%20v3.0-red.svg) ![Contributions Welcome](https://img.shields.io/badge/contributions-welcome-orange.svg) ![GitHub contributors](https://img.shields.io/github/contributors/itsmeodx/HttpServerInC) ![GitHub last commit](https://img.shields.io/github/last-commit/itsmeodx/HttpServerInC.svg) ![GitHub commit activity](https://img.shields.io/github/commit-activity/m/itsmeodx/HttpServerInC)
![GitHub issues](https://img.shields.io/github/issues/itsmeodx/HttpServerInC) ![GitHub pull requests](https://img.shields.io/github/issues-pr/itsmeodx/HttpServerInC)
![GitHub repo size](https://img.shields.io/github/repo-size/itsmeodx/HttpServerInC) ![GitHub code size](https://img.shields.io/github/languages/code-size/itsmeodx/HttpServerInC) ![GitHub language count](https://img.shields.io/github/languages/count/itsmeodx/HttpServerInC)
![GitHub top language](https://img.shields.io/github/languages/top/itsmeodx/HttpServerInC)


![GitHub Repo stars](https://img.shields.io/github/stars/itsmeodx/HttpServerInC?style=social) ![GitHub forks](https://img.shields.io/github/forks/itsmeodx/HttpServerInC?style=social) ![GitHub watchers](https://img.shields.io/github/watchers/itsmeodx/HttpServerInC?style=social)
# 🚀 HTTP Server in C

Welcome! This is a fast, minimal, and modular HTTP/1.1 server written in C. It demonstrates:
- ⚡ Networking and socket programming
- 📁 File I/O and static file serving
- 📦 HTTP protocol parsing and response formatting
- 🏷️ MIME type detection (extension-based and fallback)
- 🧩 Modular design and robust error handling

## 📖 Table of Contents
- [🚀 HTTP Server in C](#-http-server-in-c)
	- [📖 Table of Contents](#-table-of-contents)
	- [✨ Features](#-features)
	- [⚡ Quick Start](#-quick-start)
	- [⚙️ Configuration](#️-configuration)
	- [🌐 Example](#-example)
	- [📁 Structure](#-structure)
	- [📜 License](#-license)

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

## ⚙️ Configuration
- The server listens on port `4221` by default.
- The server serves files from it's working directory.
- You can change the server's working directory by passing the `--directory` option:
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
This project is licensed under the GNU General Public License v3.0. See the [LICENSE](LICENSE) file for details.

---

<div align="center">
<h4>Made with <span style="color:#e25555;">❤️</span> by <a href="https://github.com/itsmeodx">Me</a></h4>
<p><b>Feel free to contribute, report issues, or suggest improvements!</b></p>
<p>
  <a href="https://github.com/itsmeodx/HttpServerInC/issues" style="text-decoration:none;">
    <b>Check out the issues page for more information.</b>
  </a>
</p>
<sub><span style="font-size:1.2em;">Thanks for visiting! Happy coding 🚀</span></sub>
</div>

