using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Management;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows.Forms;


public struct ExeData
{
    public uint pid;
    public string title;
}

static class NativeMethods /* provides all the wrappers to the WINAPI */
{
    #region Constants
    public const uint TH32CS_SNAPPROCESS = 0x00000002;
    #endregion
    #region DllImports

    [DllImport("kernel32.dll", EntryPoint = "CloseHandle")]
    internal static extern bool _CloseHandle(IntPtr hObject);
    internal static bool CloseHandle(IntPtr hObject) { return _CloseHandle(hObject); }

    [DllImport("kernel32.dll")]
    internal static extern IntPtr CreateToolhelp32Snapshot(uint flags, uint processID);

    [DllImport("kernel32.dll")]
    internal static extern int Process32First(IntPtr snapshot, ref PROCESSENTRY32 processEntry);

    [DllImport("kernel32.dll")]
    internal static extern int Process32Next(IntPtr snapshot, ref PROCESSENTRY32 processEntry);

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
    #region Structs
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
    #endregion
    #region Enums
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
}

static class injector
{
    #region InjectDLL
    public static Boolean InjectDLL(String DLL, uint ProcID)
    {
        UIntPtr bWritten;
        IntPtr hThread;

        byte[] DLLName = new ASCIIEncoding().GetBytes(DLL);

        IntPtr hKernel = NativeMethods.LoadLibraryA("kernel32.dll");
        UIntPtr LoadLib = NativeMethods.GetProcAddress(hKernel, "LoadLibraryA");
        NativeMethods.FreeLibrary(hKernel);

        if (LoadLib == UIntPtr.Zero)
            return false;

        IntPtr ProcHandle = NativeMethods.OpenProcess(NativeMethods.ProcessAccess.AllAccess, false, (int)ProcID);

        if (ProcHandle == IntPtr.Zero)
            return false;

        IntPtr cave = NativeMethods.VirtualAllocEx(ProcHandle, (IntPtr)0, (uint)DLLName.Length, 0x1000 | 0x2000, 0x04);

        if (cave == IntPtr.Zero)
            return false;

        Boolean WPM = NativeMethods.WriteProcessMemory(ProcHandle, cave, DLLName, (uint)DLLName.Length, out bWritten);
        if (WPM == false)
            return false;

        IntPtr hThr = NativeMethods.CreateRemoteThread(ProcHandle, (IntPtr)0, (uint)0, LoadLib, cave, (uint)0, out hThread);

        if (hThr == IntPtr.Zero)
            return false;

        return true;
    }
    #endregion
    #region GetProcessIDByName
    public static uint GetProcessIDByName(String Name)
    {
        foreach (ExeData Exe in GetProcessesEx())
            if (Exe.title.Contains(Name))
                return Exe.pid;
        return 0;
    }
    #endregion
    #region GetDllsEx
    public static ArrayList GetDllsEx()
    {
        ArrayList ar = new ArrayList();
        string[] filePaths = Directory.GetFiles(Directory.GetCurrentDirectory(), "*.dll");
        foreach (string File in filePaths)
        {
            //remove directory path
            int loc = File.LastIndexOf("\\") + 1;
            ar.Add(File.Substring(loc, File.Length - loc));
        }
        return ar;
    }
    #endregion
    #region GetProcessesEx
    public static ArrayList GetProcessesEx()
    {
        ArrayList ar = new ArrayList();
        IntPtr snapshot_ = NativeMethods.CreateToolhelp32Snapshot(NativeMethods.TH32CS_SNAPPROCESS, 0);

        if (snapshot_ == IntPtr.Zero)
            return null;

        NativeMethods.PROCESSENTRY32 entry_ = new NativeMethods.PROCESSENTRY32();
        entry_.dwSize = (uint)Marshal.SizeOf(entry_);

        if (NativeMethods.Process32First(snapshot_, ref entry_) == 0)
        {
            NativeMethods.CloseHandle(snapshot_);
            return null;
        }

        do
        {
            ExeData w = new ExeData();
            w.title = entry_.szExeFile;
            w.pid = entry_.th32ProcessID;
            ar.Add(w);
        }
        while (NativeMethods.Process32Next(snapshot_, ref entry_) != 0);

        NativeMethods.CloseHandle(snapshot_);
        return ar;
    }
    #endregion

    static bool running = true;
    public static void OnApplicationExit(object sender, EventArgs e)
    {
        running = false;
    }

    public static void worker()
    {
        try
        {
            bool injected = false;
            uint PID = 0;
            while (running)
            {
                if ((PID = GetProcessIDByName("isaac-ng.exe")) == 0)
                {
                    injected = false;
                }

                if (!injected && PID != 0)
                {
                    Thread.Sleep(1000);
                    injected = InjectDLL(Directory.GetCurrentDirectory() + "\\" + "loader.dll", PID);
                }
                Thread.Sleep(1000);
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex.ToString() + "\r\n");
        }
    }
}
