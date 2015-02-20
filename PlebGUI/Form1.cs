using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Collections;
using System.Management;
using System.Diagnostics;
using System.IO;
using System.Threading;
namespace PlebGUI
{
    public partial class Form1 : Form
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool AllocConsole();

        TextWriter _writer = null;
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            _writer = new TextBoxStreamWriter(txtConsole);
            Console.SetOut(_writer);
            Console.WriteLine("heh");
        }
    }
}
