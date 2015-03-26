using System;
using System.Runtime.InteropServices;
using System.Management;
using System.Windows.Forms;

namespace gemini
{   

    public partial class mainForm : Form
    {
        public const int WM_NCLBUTTONDOWN = 0xA1;
        public const int HTCAPTION = 0x2;
        [DllImport("User32.dll")]
        public static extern bool ReleaseCapture();
        [DllImport("User32.dll")]
        public static extern int SendMessage(IntPtr hWnd, int Msg, int wParam, int lParam);

        [Flags]
        private enum tabsEnum
        {
            tabFrontNull,
            tabLoader,
            tabTracker,
            tabRaces,
            tabNull
        }
        tabsEnum tabs = tabsEnum.tabLoader;
        public mainForm()
        {
            InitializeComponent();
            this.MouseDown += _MouseDown;
        }

        private void _MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                ReleaseCapture();
                SendMessage(Handle, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            }
        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void btnMinimize_Click(object sender, EventArgs e)
        {
            this.WindowState = FormWindowState.Minimized;
        }

        private void mainForm_Resize(object sender, EventArgs e)
        {
            if (FormWindowState.Minimized == this.WindowState)
            {
                trayIcon.BalloonTipText = "Gemini Loader is still running in the background.";
                trayIcon.BalloonTipTitle = "Gemini Loader";
                trayIcon.BalloonTipIcon = ToolTipIcon.Info;
                trayIcon.Visible = true;
                trayIcon.ShowBalloonTip(500);
                this.Hide();
            }
            else if (FormWindowState.Normal == this.WindowState)
            {
                trayIcon.Visible = false;
            }
        }

        private void trayIcon_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            this.WindowState = FormWindowState.Normal;
            this.Show();
        }

        private void btnNextTab_Click(object sender, EventArgs e)
        {
            tabs += 1;
            if (tabs == tabsEnum.tabNull)
            {
                tabs = tabsEnum.tabLoader;
            }
            showProperTAB();
        }

        private void btnPrevTab_Click(object sender, EventArgs e)
        {
            tabs -= 1;
            if (tabs == tabsEnum.tabFrontNull)
            {
                tabs = tabsEnum.tabNull - 1;
            }
            showProperTAB();

        }
        private void showProperTAB()
        {
            foreach (Control c in this.Controls)
            {
                if (c.Tag != null)
                {
                    if (c.Tag.ToString().Contains("tab"))
                    {
                        c.Hide();
                    }
                    if (c.Tag.ToString() == tabs.ToString())
                    {
                        c.Show();
                    }
                }
            }
        }
    }
}
