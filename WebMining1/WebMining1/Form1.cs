using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.IO;
using System.Text.RegularExpressions;
using System.Diagnostics;

namespace WebMining1
{

    public partial class Form1 : Form
    {
        private const int k = 10;
        private const int THRESH = 4;
        private string inStr = "";

        private void display_k_most_popular_naive()
        {
            char[] space = new char[3];
            space[0] = ' ';
            space[1] = '\n';
            space[2] = '\t';
            List<string> words = new List<string>();
            foreach (string word in inStr.Split(space))
            {
                if (word.CompareTo("") != 0)
                    words.Add(word);
            }

            // Metoda moja:
            Dictionary<string, int> myDict = new Dictionary<string,int>();
                // start measure
            Stopwatch timer = Stopwatch.StartNew();
            foreach (string word in words)
                if (myDict.ContainsKey(word))
                    myDict[word]++;
                else
                    myDict.Add(word, 1);
            var sortedDict = (from entry in myDict where entry.Value >= k orderby entry.Value descending select entry).ToDictionary(pair => pair.Key, pair => pair.Value);
                // stop measure
            timer.Stop();
            string result = "Moja metoda: (" + timer.ElapsedMilliseconds + " ms)\n";
            int newK = (sortedDict.Count < k) ? sortedDict.Count : k;
            for (int i = 0; i < newK; i++)
            {
                result += "\"" + sortedDict.ElementAt(i).Key + "\" " + sortedDict.ElementAt(i).Value + " times\n";
            }

            // Metoda naiwna:
            timer = Stopwatch.StartNew();
            words.Sort();
            List<Tuple<string, int>> wordList = new List<Tuple<string, int>>();
            string pop = words[0];
            int cnt = 1;
            for (int i = 1; i < words.Count; i++)
                if (words[i].CompareTo(pop) == 0)
                    cnt++;
                else {
                    wordList.Add(new Tuple<string, int>(pop, cnt));
                    pop = words[i];
                    cnt = 1;
                }
            wordList.Add(new Tuple<string, int>(pop, cnt));
            wordList.Sort((a, b) => b.Item2.CompareTo(a.Item2));
                // stop measure
            timer.Stop();
            result += "\nMetoda naiwna: (" + timer.ElapsedMilliseconds +" ms)\n";
            newK = (wordList.Count < k) ? wordList.Count : k;
            for (int i = 0; i < newK; i++)
            {
                result += "\"" + wordList.ElementAt(i).Item1 + "\" " + wordList.ElementAt(i).Item2 + " times\n";
            }
            MessageBox.Show(result);

        }

        public Form1()
        {
            InitializeComponent();
        }

        private void saveFileDialog1_FileOk(object sender, CancelEventArgs e)
        {
            Stream fs = saveFileDialog1.OpenFile();
            StreamWriter sw = new StreamWriter(fs, Encoding.Unicode);
            try
            {
                WebClient client = new WebClient();
                Stream data = client.OpenRead(textBox1.Text);
                StreamReader reader = new StreamReader(data);

                // Get HTML data
                string str = "";
                str = reader.ReadLine();

                while (str != null)
                {
                    sw.WriteLine(str);
                    str = reader.ReadLine();
                }
                data.Close();
            }
            catch (WebException exp)
            {
                MessageBox.Show(exp.Message, "Exception");
            }
            finally
            {
                sw.Flush();
                fs.Close();
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            saveFileDialog1.ShowDialog();
            textBox2.Text = saveFileDialog1.FileName;
        }

        private void saveFileDialog2_FileOk(object sender, CancelEventArgs e)
        {
            Stream s = saveFileDialog2.OpenFile();
            StreamWriter sw = new StreamWriter(s);
//            Regex rc = new Regex("<!--[^?]*-->");
//            Regex rn = new Regex("<[^>]+>");
            FileStream fs = new FileStream(textBox2.Text, FileMode.Open);
            StreamReader sr = new StreamReader(fs, Encoding.Unicode);
            inStr = "";
            try
            {
                string str = "";
                str = sr.ReadLine();
                while (str != null)
                {
                    inStr += str + "\n";
                    str = sr.ReadLine();
                }

/*                Match m;
                // remove comments
                m = rc.Match(inStr);
                while (m.Success)
                {
                    inStr = inStr.Substring(0, m.Index) + inStr.Substring(m.Index + m.Length);
                    m = rc.Match(inStr);
                }

                // remove nodes
                m = rn.Match(inStr);
                while (m.Success)
                {
                    inStr = inStr.Substring(0, m.Index) + inStr.Substring(m.Index + m.Length);
                    m = rn.Match(inStr);
                }

  */
                int chrCnt = 0;
                int pCnt = 0, ppCnt = 0, ptCnt = 0;
                int chrPos = -1;
                int insideScript = 0;
                for (int p = 0; p < inStr.Length; p++)
                {
                    switch (inStr[p])
                    {
                        case '↑':
                            inStr = inStr.Substring(0, p) + inStr.Substring(p + 1);
                            p--;
                            break;
                        case '<':
                            if (p + 6 < inStr.Length)
                                if (inStr.Substring(p, 7).CompareTo("<script") == 0)
                                {
                                    insideScript ^= 1;
                                }
                            if ((pCnt | ppCnt | ptCnt) == 0)
                            {
                                if (chrCnt == 0)
                                    chrPos = p;
                                chrCnt++;
                            }
                            break;
                        case '>':
                            if ((pCnt | ppCnt | ptCnt) == 0)
                            {
                                chrCnt--;
                                if (chrCnt == 0)
                                {
                                    inStr = inStr.Substring(0, chrPos) + inStr.Substring(p + 1);
                                    p = chrPos - 1;
                                }
                            }
                            break;
                        case '\'':
                            if(insideScript != 0)
                                pCnt ^= 1;
                            break;
                        case '\"':
                            if(insideScript != 0)
                                ppCnt ^= 1;
                            break;
                        case '(':
                            if ((insideScript != 0) && ((pCnt & ppCnt) == 0))
                                ptCnt++;
                            break;
                        case ')':
                            if ((insideScript != 0) && ((pCnt & ppCnt) == 0))
                                ptCnt--;
                            break;
                    }
                }

                inStr = new string(inStr.Where(c => (!char.IsPunctuation(c))).ToArray()).ToLower();

                sw.Write(inStr);

                // display k most popular words
                display_k_most_popular_naive();
            }
            finally
            {
                sw.Flush();
                fs.Close();
                s.Close();
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            saveFileDialog2.ShowDialog();
        }
    }
}
