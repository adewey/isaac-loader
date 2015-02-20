using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Collections;
using System.Management;
using System.Text;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Threading;

namespace PlebGUI
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Injector injectorObj = new Injector();
            Thread injectorThread = new Thread(injectorObj.DoInjection);
            injectorThread.Start();
            Application.Run(new Form1());
            injectorObj.Stop();
        }
    }
    public class Injector
    {
        public const uint TH32CS_SNAPPROCESS = 0x00000002;
        #region DllImports

        [DllImport("kernel32.dll", EntryPoint = "CloseHandle")]
        private static extern bool _CloseHandle(IntPtr hObject);
        private static bool CloseHandle(IntPtr hObject) { return _CloseHandle(hObject); }

        [DllImport("kernel32.dll")]
        private static extern IntPtr CreateToolhelp32Snapshot(uint flags, uint processID);

        [DllImport("kernel32.dll")]
        private static extern int Process32First(IntPtr snapshot, ref PROCESSENTRY32 processEntry);

        [DllImport("kernel32.dll")]
        private static extern int Process32Next(IntPtr snapshot, ref PROCESSENTRY32 processEntry);

        [DllImport("kernel32")]
        private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);

        [DllImport("kernel32")]
        public static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);


        [DllImport("kernel32", SetLastError = true, CharSet = CharSet.Ansi)]
        internal static extern IntPtr LoadLibraryA(string lpFileName);

        [DllImport("kernel32", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
        internal static extern UIntPtr GetProcAddress(IntPtr hModule, string procName);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool FreeLibrary(IntPtr hModule);

        [DllImport("kernel32.dll")]
        internal static extern IntPtr OpenProcess(ProcessAccess dwDesiredAccess, [MarshalAs(UnmanagedType.Bool)] bool bInheritHandle, int dwProcessId);

        [DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
        internal static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress,
           uint dwSize, UInt32 flAllocationType, UInt32 flProtect);

        [DllImport("kernel32.dll", SetLastError = true)]
        internal static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, uint nSize, out UIntPtr lpNumberOfBytesWritten);

        [DllImport("kernel32.dll")]
        internal static extern IntPtr CreateRemoteThread(IntPtr hProcess, IntPtr lpThreadAttributes, uint dwStackSize, UIntPtr lpStartAddress, IntPtr lpParameter, uint dwCreationFlags, out IntPtr lpThreadId);

        [DllImport("kernel32.dll", SetLastError = true)]
        internal static extern bool ReadProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, [Out] byte[] lpBuffer, int dwSize, out int lpNumberOfBytesRead);
        #endregion
        #region Injector
        public static Boolean InjectDLL(String DLL, uint ProcID)
        {
            UIntPtr bWritten;
            IntPtr hThread;

            byte[] DLLName = new ASCIIEncoding().GetBytes(DLL);

            IntPtr hKernel = LoadLibraryA("kernel32.dll");
            UIntPtr LoadLib = GetProcAddress(hKernel, "LoadLibraryA");
            FreeLibrary(hKernel);

            if (LoadLib == UIntPtr.Zero)
            {
                Console.WriteLine("1");
                return false;
            }

            IntPtr ProcHandle = OpenProcess(ProcessAccess.AllAccess, false, (int)ProcID);

            if (ProcHandle == IntPtr.Zero)
            {
                Console.WriteLine("2");
                return false;
            }

            IntPtr cave = VirtualAllocEx(ProcHandle, (IntPtr)0, (uint)DLLName.Length, 0x1000 | 0x2000, 0x04);

            if (cave == IntPtr.Zero)
            {
                Console.WriteLine("3");
                return false;
            }
                

            Boolean WPM = WriteProcessMemory(ProcHandle, cave, DLLName, (uint)DLLName.Length, out bWritten);
            if (WPM == false)
            {
                Console.WriteLine("4");
                return false;
            }
                

            IntPtr hThr = CreateRemoteThread(ProcHandle, (IntPtr)0, (uint)0, LoadLib, cave, (uint)0, out hThread);

            if (hThr == IntPtr.Zero)
            {
                Console.WriteLine("5");
                return false;
            }

            return true;
        }
        public static uint GetProcessIDByName(String Name)
        {
            foreach (ExeData Exe in GetProcessesEx())
            {
                if (Exe.title.Contains(Name))
                {
                    return Exe.pid;
                }
            }
            return 0;
        }
        public static ArrayList GetProcessesEx()
        {
            ArrayList ar = new ArrayList();
            IntPtr snapshot_ = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

            if (snapshot_ == IntPtr.Zero)
            {
                return null;
            }

            PROCESSENTRY32 entry_ = new PROCESSENTRY32();
            entry_.dwSize = (uint)Marshal.SizeOf(entry_);

            if (Process32First(snapshot_, ref entry_) == 0)
            {
                CloseHandle(snapshot_);
                return null;
            }

            do
            {
                ExeData w = new ExeData();
                w.title = entry_.szExeFile;
                w.pid = entry_.th32ProcessID;
                ar.Add(w);
            }
            while (Process32Next(snapshot_, ref entry_) != 0);

            CloseHandle(snapshot_);
            return ar;
        }
        public struct ExeData
        {
            public uint pid;
            public string title;
        }
        [StructLayoutAttribute(LayoutKind.Sequential)]
        public struct PROCESSENTRY32
        {
            public uint dwSize;
            public uint cntUsage;
            public uint th32ProcessID;
            public IntPtr th32DefaultHeapID;
            public uint th32ModuleID;
            public uint cntThreads;
            public uint th32ParentProcessID;
            public int pcPriClassBase;
            public uint dwFlags;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
            public string szExeFile;
        };
        public enum ProcessAccess : int
        {
            /// <summary>Specifies all possible access flags for the process object.</summary>
            AllAccess = CreateThread | DuplicateHandle | QueryInformation | SetInformation | Terminate | VMOperation | VMRead | VMWrite | Synchronize,
            /// <summary>Enables usage of the process handle in the CreateRemoteThread function to create a thread in the process.</summary>
            CreateThread = 0x2,
            /// <summary>Enables usage of the process handle as either the source or target process in the DuplicateHandle function to duplicate a handle.</summary>
            DuplicateHandle = 0x40,
            /// <summary>Enables usage of the process handle in the GetExitCodeProcess and GetPriorityClass functions to read information from the process object.</summary>
            QueryInformation = 0x400,
            /// <summary>Enables usage of the process handle in the SetPriorityClass function to set the priority class of the process.</summary>
            SetInformation = 0x200,
            /// <summary>Enables usage of the process handle in the TerminateProcess function to terminate the process.</summary>
            Terminate = 0x1,
            /// <summary>Enables usage of the process handle in the VirtualProtectEx and WriteProcessMemory functions to modify the virtual memory of the process.</summary>
            VMOperation = 0x8,
            /// <summary>Enables usage of the process handle in the ReadProcessMemory function to' read from the virtual memory of the process.</summary>
            VMRead = 0x10,
            /// <summary>Enables usage of the process handle in the WriteProcessMemory function to write to the virtual memory of the process.</summary>
            VMWrite = 0x20,
            /// <summary>Enables usage of the process handle in any of the wait functions to wait for the process to terminate.</summary>
            Synchronize = 0x100000
        }
        #endregion
        Boolean keepGoing = true;
        Boolean injected = false;
        public void DoInjection()
        {
            Console.WriteLine("SADFASDFASD");
            injected = false;
            while (keepGoing)
            {
                uint dwPid = GetProcessIDByName("isaac-ng.exe");
                if (dwPid != 0 && !injected)
                {
                    /* let the game load a little before trying to inject */
                    System.Threading.Thread.Sleep(100);
                    injected = InjectDLL(Directory.GetCurrentDirectory() + "\\" + "loader.dll", GetProcessIDByName("isaac-ng.exe"));
                    Console.WriteLine(injected);
                }
                else if (dwPid == 0 && injected)
                {
                    Console.WriteLine("Shite");
                    injected = !injected;
                }
                System.Threading.Thread.Sleep(900);
            }
        }
        public void Stop()
        {
            Console.WriteLine("K");
            keepGoing = false;
        }
    }
}
