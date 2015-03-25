using System;
using System.Threading;
using System.Windows.Forms;

namespace gemini
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            // create our thread to handle injecting into the executable
            new Thread(injector.worker).Start();

            Application.ApplicationExit += injector.OnApplicationExit;
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new mainForm());
        }
    }
}
