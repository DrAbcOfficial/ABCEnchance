// See https://aka.ms/new-console-template for more information
using Steamworks;

static void CopyFolder(string source, string destination)
{
    Directory.CreateDirectory(destination);

    foreach (string file in Directory.EnumerateFiles(source))
    {
        string fileName = Path.GetFileName(file);
        string destFile = Path.Combine(destination, fileName);
        try
        {
            File.Copy(file, destFile, true);
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Copied {0} ===> {1}", fileName, destFile);
            Console.ForegroundColor = ConsoleColor.Gray;
        }
        catch(Exception ex) 
        { 
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine(ex.Message);
            Console.ForegroundColor = ConsoleColor.Gray;
        }
    }

    foreach (string folder in Directory.EnumerateDirectories(source))
    {
        string folderName = Path.GetFileName(folder);
        string destFolder = Path.Combine(destination, folderName);
        CopyFolder(folder, destFolder);
    }
}
static void FileWriter(string file, string add, string ignored = "")
{
    List<string> filelist = new();
    using (StreamReader sr = new StreamReader(file))
    {
        while (!sr.EndOfStream)
        {
            string? line = sr.ReadLine();
            if (line == null)
                continue;
            line = line.Trim();
            if (line == add || line == ignored)
            {
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine("Ignored {0}...", line);
                Console.ForegroundColor = ConsoleColor.Gray;
                continue;
            }  
            filelist.Add(line);
        }
    }
    filelist.Insert(0, add);
    using (StreamWriter sw = new StreamWriter(file))
    {
        foreach (string line in filelist)
        {
            sw.WriteLine(line);
        }
    }
}

Console.WriteLine("Hello, World!");
Environment.SetEnvironmentVariable("SteamAppId", "225840");
if (!SteamAPI.Init())
{
    Console.WriteLine("Steamworks.NET could not be initialized.");
    return;
}
AppId_t appId = new(225840);
string svenPath = "";
uint result = SteamApps.GetAppInstallDir(appId, out svenPath, 260);
if (result == 0)
{
    Console.WriteLine("Steamworks.NET could not init game.");
    return;
}
CopyFolder("./svencoop", svenPath + "/svencoop");
CopyFolder("./svencoop_addon", svenPath + "/svencoop_addon");

FileWriter(svenPath + "/svencoop/metahook/configs/dllpaths.lst", "vpx");
FileWriter(svenPath + "/svencoop/metahook/configs/plugins.lst", "ABCEnchance.dll", "CommunicationDemo.dll");