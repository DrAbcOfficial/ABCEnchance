﻿using Steamworks;
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
        catch (Exception ex)
        {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine(ex.Message);
            Console.ForegroundColor = ConsoleColor.Gray;
        }
    }
    foreach (string folder in Directory.EnumerateDirectories(source))
    {
        CopyFolder(folder, Path.Combine(destination, Path.GetFileName(folder)));
    }
}
static void FileWriter(string file, string add, string ignored = "")
{
    try
    {
        List<string> filelist = new();
        using (StreamReader sr = new(file))
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
        using StreamWriter sw = new(file);
        foreach (string line in filelist)
        {
            sw.WriteLine(line);
        }
    }
    catch (Exception ex)
    {
        Console.ForegroundColor = ConsoleColor.Red;
        Console.WriteLine("Exception when write file: {0}", file);
        Console.WriteLine(ex.Message);
        Console.WriteLine("Please edit it manually");
        Console.ForegroundColor = ConsoleColor.Gray;
        System.Diagnostics.Process.Start("explorer.exe", file);

    }
}
static void AnyKeyExit(string msg)
{
    Console.ForegroundColor = ConsoleColor.Cyan;
    Console.WriteLine(msg);
    Console.WriteLine("Any Key Exit...");
    Console.ReadKey();
}
Environment.SetEnvironmentVariable("SteamAppId", "225840");
if (!SteamAPI.Init())
{
    AnyKeyExit("Steamworks.NET could not be initialized.");
    return;
}
string svenPath;
uint result = SteamApps.GetAppInstallDir(new AppId_t(225840), out svenPath, 260);
if (result == 0)
{
    AnyKeyExit("Steamworks.NET could not init game.");
    return;
}

string localPath = Path.GetFullPath(".");
if(svenPath.Contains(localPath, StringComparison.OrdinalIgnoreCase))
{
    AnyKeyExit("Oops, are you put installer into your game folder? put them to another place.");
    return;
}

if (!Path.Exists(svenPath + "/svencoop/metahook/configs/plugins.lst"))
{
    AnyKeyExit("It looks like your metahook is not installed correctly, please make sure the metahook is installed in the right place!");
    return;
}

CopyFolder("./svencoop", svenPath + "/svencoop");
CopyFolder("./svencoop_addon", svenPath + "/svencoop_addon");
CopyFolder("./svencoop_hidpi", svenPath + "/svencoop_hidpi");
FileWriter(svenPath + "/svencoop/metahook/configs/plugins.lst", "ABCEnchance.dll", "CommunicationDemo.dll");
AnyKeyExit("Done! you can remove installer folder safly.");