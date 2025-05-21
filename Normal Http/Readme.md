# TCP Socket Server in C - README

This project demonstrates a simple TCP socket server written in C, designed to run on a Linux environment (such as Ubuntu via WSL 2). It listens for incoming client connections on port 8000, receives an HTTP request, and sends back a simple HTML response.

---

## 🛠️ Requirements

* **Operating System:** Linux (recommended via WSL 2 if using Windows)
* **Compiler & Tools:** GCC (GNU Compiler Collection)
* **Libraries:**

  * Standard I/O (`stdio.h`)
  * Sockets (`sys/socket.h`)
  * Internet address structures (`netinet/in.h`)
  * POSIX OS functions (`unistd.h`)
  * String utilities (`string.h`)

---

## ⚙️ Installation & Setup

### Step 1: Enable WSL 2 (for Windows users)

Make sure WSL 2 is properly set up and a Linux distribution like Ubuntu is installed.

### Step 2: Install VS Code & Remote WSL Extension

* Download [Visual Studio Code](https://code.visualstudio.com/)
* Install the **Remote - WSL** extension to open and run files directly in Ubuntu

### Step 3: Install Required Tools

Open the terminal in Ubuntu (inside VS Code) and run:

```bash
sudo apt update
sudo apt install build-essential
```

This command installs:

* `gcc` (C compiler)
* `g++` (C++ compiler)
* Standard development libraries and headers

### Step 4: Compile the Server

```bash
gcc server.c -o server
```

### Step 5: Run the Server

```bash
./server
```

The server will now start and listen on port 8000.

---

## 🌐 Testing the Server

Once the server is running, open a browser or use `curl`:

```bash
curl http://localhost:8000
```

You should see:

```html
<!DOCTYPE html>
<html> <body> <h1> Hello From Server </h1> </body> </html>
```

---

## 💡 Notes

1. **Why WSL 2 is Required on Windows:**

   * Windows does not natively support the Linux-style `<sys/socket.h>` headers.
   * WSL 2 gives you a full Linux kernel and environment.

2. **Include Paths in Linux:**

   * Ubuntu and other Linux distros use their own include paths for system libraries.
   * Installing `build-essential` sets up all required headers and compilers.

3. **Common Ports:**

   * The server uses port `8000`, but you can change it as needed.
   * `localhost:8000` works the same in both Ubuntu and Windows because of shared networking in WSL 2.

---

## 📂 Files

* `server.c` : The main server program
* `README.md` : This documentation

---

## 🧠 How It Works (Short Summary)

1. Create a socket with `socket()`
2. Bind it to IP & Port using `bind()`
3. Mark it as passive with `listen()`
4. Accept incoming client connections using `accept()`
5. Receive client data with `recv()`
6. Respond with an HTTP message using `send()`
7. Close the connection with `close()`

---

## 🔐 License

Free to use for learning and educational purposes.
