using System;
using System.Runtime.InteropServices;
using System.Management;
using System.Windows.Forms;
using System.Collections;
using Newtonsoft.Json.Linq;
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

        ArrayList tabList = new ArrayList();
        public mainForm()
        {
            InitializeComponent();
            String tempJson = "{type:\"textBox\",fun:\"no\", width:\"300\", height:\"15\", left:\"50\", top:\"100\", tab:\"tabRaces\", name:\"racesTxt\" }";
            JObject tempJObj = JObject.Parse(tempJson);
            addControlToTab(tempJObj);

            tempJson = "{type:\"label\",fun:\"no\", width:\"300\", height:\"12\", left:\"50\", top:\"90\", tab:\"tabGolf\", name:\"golfLbl\", text:\"Golf Tab\" }";
            tempJObj = JObject.Parse(tempJson);
            addControlToTab(tempJObj);

            tempJson = "{type:\"textBox\",fun:\"no\", width:\"100\", height:\"12\", left:\"50\", top:\"120\", tab:\"tabGolf\", name:\"racesTxt\" }";
            tempJObj = JObject.Parse(tempJson);
            addControlToTab(tempJObj);

            tempJson = "{type:\"button\", width:\"300\", height:\"100\", left:\"50\", top:\"90\", tab:\"tabButton\", name:\"testBtn\", text:\"New Button\", action:\"'racesTxt','golfLbl'\" }";
            tempJObj = JObject.Parse(tempJson);
            addControlToTab(tempJObj);
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

        private void btnNextTab_Click(object sender, EventArgs e){
            showProperTAB();
        }

        private void btnPrevTab_Click(object sender, EventArgs e){
            showProperTAB();
        }

        private void showProperTAB(){
            Console.WriteLine(tabList.ToString());
            tabList.Add(tabList[0]);
            tabList.RemoveAt(0);
            tabList.TrimToSize();
            foreach (Control c in this.Controls){
                if (c.Tag != null){
                    if (c.Tag.ToString().Contains("tab")){
                        c.Hide();
                    }
                    if (c.Tag.ToString().Contains(tabList[0].ToString())){
                        c.Show();
                    }
                }
            }
        }
        private void addControlToTab(JObject ctrlParams){
            if (ctrlParams.GetValue("tab") == null)
            {
                MessageBox.Show("Attempt to create control without specifiying tab.","Error: ", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            if(!tabList.Contains(ctrlParams.GetValue("tab")))
            {
                tabList.Add(ctrlParams.GetValue("tab"));
            }

            int top;
            int left;
            int width;
            int height;
            String name;
            String text;
            String tab = ctrlParams.GetValue("tab").ToString();
            String action;
            JToken tempTok;
            switch (ctrlParams.GetValue("type").ToString().ToLower()){
                case "textbox":
                    #region "Validation";
                    tempTok = ctrlParams.GetValue("width");
                    if(!(tempTok != null && int.TryParse(tempTok.ToString(),out width)))
                    {
                        MessageBox.Show("Attempt to Create TextBox with improper or no width.", "Error:", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        break;
                    }
                    
                    tempTok = ctrlParams.GetValue("height");
                    if(!(tempTok != null && int.TryParse(tempTok.ToString(),out height)))
                    {
                        MessageBox.Show("Attempt to Create TextBox with improper or no height.", "Error:", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        break;
                    }

                    tempTok = ctrlParams.GetValue("left");
                    if(!(tempTok != null && int.TryParse(tempTok.ToString(),out left)))
                    {
                        MessageBox.Show("Attempt to Create TextBox with improper or no left.", "Error:", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        break;
                    }
                    
                    tempTok = ctrlParams.GetValue("top");
                    if(!(tempTok != null && int.TryParse(tempTok.ToString(),out top)))
                    {
                        MessageBox.Show("Attempt to Create TextBox with improper or no top.", "Error:", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        break;
                    }

                    tempTok = ctrlParams.GetValue("name");
                    if (tempTok != null)
                    {
                        name = tempTok.ToString();
                    }
                    else
                    {
                        MessageBox.Show("Attempt to Create TextBox with improper or no name.", "Error:", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        break;
                    }
                    #endregion

                    TextBox newTxt = new TextBox();
                    newTxt.Left = left;
                    newTxt.Width = width;
                    newTxt.Top = top;
                    newTxt.Height = height;
                    newTxt.Tag = tab;
                    newTxt.Name = name;
                    newTxt.Visible = false;
                    this.Controls.Add(newTxt);
                    newTxt.BringToFront();
                break;
                case "label":
                #region "Validation"
                    tempTok = ctrlParams.GetValue("width");
                    if(!(tempTok != null && int.TryParse(tempTok.ToString(),out width)))
                    {
                        MessageBox.Show("Attempt to Create TextBox with improper or no width.", "Error:", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        break;
                    }
                    
                    tempTok = ctrlParams.GetValue("height");
                    if(!(tempTok != null && int.TryParse(tempTok.ToString(),out height)))
                    {
                        MessageBox.Show("Attempt to Create TextBox with improper or no height.", "Error:", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        break;
                    }

                    tempTok = ctrlParams.GetValue("left");
                    if(!(tempTok != null && int.TryParse(tempTok.ToString(),out left)))
                    {
                        MessageBox.Show("Attempt to Create TextBox with improper or no left.", "Error:", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        break;
                    }
                    
                    tempTok = ctrlParams.GetValue("top");
                    if(!(tempTok != null && int.TryParse(tempTok.ToString(),out top)))
                    {
                        MessageBox.Show("Attempt to Create TextBox with improper or no top.", "Error:", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        break;
                    }

                    tempTok = ctrlParams.GetValue("name");
                    if (tempTok != null)
                    {
                        name = tempTok.ToString();
                    }
                    else
                    {
                        MessageBox.Show("Attempt to Create TextBox with improper or no name.", "Error:", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        break;
                    }
                    tempTok = ctrlParams.GetValue("text");
                    if (tempTok != null)
                    {
                        text = tempTok.ToString();
                    }
                    else
                    {
                        MessageBox.Show("Attempt to Create TextBox with no text.", "Error:", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        break;
                    }
                #endregion

                    Label newLbl = new Label();
                    newLbl.Left = left;
                    newLbl.Width = width;
                    newLbl.Top = top;
                    newLbl.Height = height;
                    newLbl.Tag = tab;
                    newLbl.Name = name;
                    newLbl.Visible = false;
                    newLbl.Text = text;
                    this.Controls.Add(newLbl);
                    newLbl.BringToFront();
                break;
                case "button":
                #region "Validation"
                    
                tempTok = ctrlParams.GetValue("width");
                if (!(tempTok != null && int.TryParse(tempTok.ToString(), out width)))
                {
                    MessageBox.Show("Attempt to Create TextBox with improper or no width.", "Error:", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    break;
                }

                tempTok = ctrlParams.GetValue("height");
                if (!(tempTok != null && int.TryParse(tempTok.ToString(), out height)))
                {
                    MessageBox.Show("Attempt to Create TextBox with improper or no height.", "Error:", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    break;
                }

                tempTok = ctrlParams.GetValue("left");
                if (!(tempTok != null && int.TryParse(tempTok.ToString(), out left)))
                {
                    MessageBox.Show("Attempt to Create TextBox with improper or no left.", "Error:", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    break;
                }

                tempTok = ctrlParams.GetValue("top");
                if (!(tempTok != null && int.TryParse(tempTok.ToString(), out top)))
                {
                    MessageBox.Show("Attempt to Create TextBox with improper or no top.", "Error:", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    break;
                }

                tempTok = ctrlParams.GetValue("name");
                if (tempTok != null)
                {
                    name = tempTok.ToString();
                }
                else
                {
                    MessageBox.Show("Attempt to Create TextBox with improper or no name.", "Error:", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    break;
                }
                tempTok = ctrlParams.GetValue("text");
                if (tempTok != null)
                {
                    text = tempTok.ToString();
                }
                else
                {
                    MessageBox.Show("Attempt to Create TextBox with no text.", "Error:", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    break;
                }

                tempTok = ctrlParams.GetValue("action");
                if (tempTok != null)
                {
                    action = tempTok.ToString();
                }
                else
                {
                    action = "";
                }
                #endregion
                    
                Button newBtn = new Button();
                newBtn.Left = left;
                newBtn.Width = width;
                newBtn.Top = top;
                newBtn.Height = height;
                newBtn.Tag = tab;
                newBtn.Tag += " [" + action + "]";
                newBtn.Name = name;
                newBtn.Visible = false;
                newBtn.Text = text;
                newBtn.Click += HandleDynamicButtons;
                this.Controls.Add(newBtn);
                newBtn.BringToFront();
                break;
            }
        }

        void HandleDynamicButtons(object sender, EventArgs e){
            Button btn = (Button)sender;
            String tag = btn.Tag.ToString();
            int dataStart = tag.IndexOf("[");
            int dataEnd = tag.IndexOf("]");
            String data = (tag.Substring(dataStart, dataEnd - (dataStart - 1)));
            JArray ctrlArr = JArray.Parse(data);
            foreach (var obj in ctrlArr.Children())
            {
                foreach (Control C in this.Controls)
                {
                    if (C.Name == obj.ToString())
                    {
                        if (C.GetType().IsInstanceOfType(new TextBox()))
                        {

                        }
                        else if(C.GetType().IsInstanceOfType(new Label()))
                        {

                        }
                        //Switch Statement to pull the values expected depending on type of control it is
                        break;
                    }
                }
            }
        }
    }
}