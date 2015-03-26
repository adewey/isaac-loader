namespace gemini
{
    partial class mainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(mainForm));
            this.btnCloseTemp = new System.Windows.Forms.Button();
            this.btnMinimize = new System.Windows.Forms.Button();
            this.trayIcon = new System.Windows.Forms.NotifyIcon(this.components);
            this.btnNextTab = new System.Windows.Forms.Button();
            this.btnPrevTab = new System.Windows.Forms.Button();
            this.lblLoaderTab = new System.Windows.Forms.Label();
            this.btnTrackerTab = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // btnCloseTemp
            // 
            this.btnCloseTemp.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.btnCloseTemp.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.btnCloseTemp.Location = new System.Drawing.Point(271, 25);
            this.btnCloseTemp.Name = "btnCloseTemp";
            this.btnCloseTemp.Size = new System.Drawing.Size(75, 23);
            this.btnCloseTemp.TabIndex = 1;
            this.btnCloseTemp.Text = "Close";
            this.btnCloseTemp.UseVisualStyleBackColor = false;
            this.btnCloseTemp.Click += new System.EventHandler(this.btnClose_Click);
            // 
            // btnMinimize
            // 
            this.btnMinimize.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.btnMinimize.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnMinimize.Location = new System.Drawing.Point(181, 25);
            this.btnMinimize.Name = "btnMinimize";
            this.btnMinimize.Size = new System.Drawing.Size(75, 23);
            this.btnMinimize.TabIndex = 2;
            this.btnMinimize.Text = "Minimize";
            this.btnMinimize.UseVisualStyleBackColor = false;
            this.btnMinimize.Click += new System.EventHandler(this.btnMinimize_Click);
            // 
            // trayIcon
            // 
            this.trayIcon.Icon = ((System.Drawing.Icon)(resources.GetObject("trayIcon.Icon")));
            this.trayIcon.Text = "Gemini Loader";
            this.trayIcon.Visible = true;
            this.trayIcon.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.trayIcon_MouseDoubleClick);
            // 
            // btnNextTab
            // 
            this.btnNextTab.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.btnNextTab.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.btnNextTab.Location = new System.Drawing.Point(193, 364);
            this.btnNextTab.Name = "btnNextTab";
            this.btnNextTab.Size = new System.Drawing.Size(53, 23);
            this.btnNextTab.TabIndex = 4;
            this.btnNextTab.Text = "->";
            this.btnNextTab.UseVisualStyleBackColor = false;
            this.btnNextTab.Click += new System.EventHandler(this.btnNextTab_Click);
            // 
            // btnPrevTab
            // 
            this.btnPrevTab.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.btnPrevTab.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.btnPrevTab.Location = new System.Drawing.Point(26, 364);
            this.btnPrevTab.Name = "btnPrevTab";
            this.btnPrevTab.Size = new System.Drawing.Size(53, 23);
            this.btnPrevTab.TabIndex = 5;
            this.btnPrevTab.Text = "<-";
            this.btnPrevTab.UseVisualStyleBackColor = false;
            this.btnPrevTab.Click += new System.EventHandler(this.btnPrevTab_Click);
            // 
            // lblLoaderTab
            // 
            this.lblLoaderTab.AutoSize = true;
            this.lblLoaderTab.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(230)))), ((int)(((byte)(221)))), ((int)(((byte)(220)))));
            this.lblLoaderTab.Font = new System.Drawing.Font("Microsoft Sans Serif", 27.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblLoaderTab.Location = new System.Drawing.Point(74, 81);
            this.lblLoaderTab.Name = "lblLoaderTab";
            this.lblLoaderTab.Size = new System.Drawing.Size(118, 42);
            this.lblLoaderTab.TabIndex = 6;
            this.lblLoaderTab.Tag = "tabLoader";
            this.lblLoaderTab.Text = "label1";
            // 
            // btnTrackerTab
            // 
            this.btnTrackerTab.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.btnTrackerTab.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.btnTrackerTab.Font = new System.Drawing.Font("Microsoft Sans Serif", 27.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnTrackerTab.Location = new System.Drawing.Point(81, 108);
            this.btnTrackerTab.Name = "btnTrackerTab";
            this.btnTrackerTab.Size = new System.Drawing.Size(225, 94);
            this.btnTrackerTab.TabIndex = 7;
            this.btnTrackerTab.Tag = "tabTracker";
            this.btnTrackerTab.Text = "Tracker :D";
            this.btnTrackerTab.UseVisualStyleBackColor = false;
            this.btnTrackerTab.Visible = false;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(230)))), ((int)(((byte)(221)))), ((int)(((byte)(220)))));
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 64F);
            this.label1.Location = new System.Drawing.Point(49, 143);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(286, 97);
            this.label1.TabIndex = 8;
            this.label1.Tag = "tabRaces";
            this.label1.Text = "Races";
            this.label1.Visible = false;
            // 
            // pictureBox1
            // 
            this.pictureBox1.BackColor = System.Drawing.Color.CadetBlue;
            this.pictureBox1.BackgroundImage = global::gemini.Properties.Resources.GaminiLoader2x1;
            this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.pictureBox1.Enabled = false;
            this.pictureBox1.Image = global::gemini.Properties.Resources.portrait_79_0_gemini;
            this.pictureBox1.Location = new System.Drawing.Point(0, 0);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Padding = new System.Windows.Forms.Padding(222, 240, 0, 0);
            this.pictureBox1.Size = new System.Drawing.Size(388, 448);
            this.pictureBox1.TabIndex = 0;
            this.pictureBox1.TabStop = false;
            // 
            // mainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Fuchsia;
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.ClientSize = new System.Drawing.Size(386, 448);
            this.ControlBox = false;
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btnTrackerTab);
            this.Controls.Add(this.lblLoaderTab);
            this.Controls.Add(this.btnPrevTab);
            this.Controls.Add(this.btnNextTab);
            this.Controls.Add(this.btnMinimize);
            this.Controls.Add(this.btnCloseTemp);
            this.Controls.Add(this.pictureBox1);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(386, 448);
            this.MinimumSize = new System.Drawing.Size(386, 448);
            this.Name = "mainForm";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Gemini Mod Loader";
            this.TransparencyKey = System.Drawing.Color.CadetBlue;
            this.Resize += new System.EventHandler(this.mainForm_Resize);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Button btnCloseTemp;
        private System.Windows.Forms.Button btnMinimize;
        private System.Windows.Forms.NotifyIcon trayIcon;
        private System.Windows.Forms.Button btnNextTab;
        private System.Windows.Forms.Button btnPrevTab;
        private System.Windows.Forms.Label lblLoaderTab;
        private System.Windows.Forms.Button btnTrackerTab;
        private System.Windows.Forms.Label label1;

    }
}

