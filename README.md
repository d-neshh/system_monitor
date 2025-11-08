# System Monitor Tool

A lightweight cross-platform system monitoring CLI tool built in C++ that provides real-time process information and management capabilities.

## 🚀 Features

- **Process Monitoring:**
  - Process ID (PID)
  - CPU Usage (%)
  - Memory Usage (%)
  - Process Name
- **Process Management:**
  - Sort by CPU/Memory usage
  - Kill processes via PID
  - Auto-refresh monitoring
- **Platform Support:**
  - Linux (native)
  - Windows (via WSL)

## 🔧 Prerequisites

### Linux / WSL
- GCC/G++ compiler
- Linux environment or WSL2

### Windows
- Windows 10/11
- Visual Studio 2019+ with C++ workload, or
- Windows Subsystem for Linux (WSL2)

## 📥 Installation

```bash
# Clone the repository
git clone https://github.com/d-neshh/system-monitor.git

# Navigate to project directory
cd system-monitor

# Build the project
g++ system_monitor.cpp -o system_monitor
```

## 💻 Usage

```bash
# Run the program
./system_monitor
```

### Available Commands
- `1` - Sort processes by CPU usage
- `2` - Sort processes by Memory usage
- `3` - Kill a process (requires PID)
- `r` - Enable auto-refresh mode (3s interval)
- `q` - Quit the program

## 🤝 Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request



## 🙏 Acknowledgments

- Linux proc filesystem documentation
- C++ Standard Library
