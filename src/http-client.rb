require 'rubygems'
require 'json'
require 'net/http'

uri = '/rate?destination=14195551212&source=123456789&length=86400'
client = Net::HTTP.new("localhost", 9090)
string = client.get(uri, {'Accept' => 'application/json'})
begin
    parsed = JSON.parse(string.body)
    print parsed
rescue JSON::ParserError
    print "Error parsing returned value."
    raise
end
