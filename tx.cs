namespace tx
{
    class Program
    {
        static void Main(string[] args)
        {
            var txn = args[0];
            using (var client = new HttpClient(new HttpClientHandler { AutomaticDecompression = DecompressionMethods.GZip | DecompressionMethods.Deflate }))
            {
                client.BaseAddress = new Uri("https://blockchain.info/rawtx/" + txn);
                HttpResponseMessage response = client.GetAsync("").Result;
                string result = response.Content.ReadAsStringAsync().Result;
                Console.WriteLine(result);
            }
        }  
    }
}