
-- lua table function additions

-- sorts the table and removes all duplicates
function table.unique(t)
	table.sort(t)
	local n = #t
	for i = 1, n do
		if t[n-i] == t[n-i+1] then
			table.remove(t, n-i+1)
		end
	end
	return t
end

-- returns the index of elem in the table, returns 0 if nonexistent
function table.find(t, elem)
	for i = 1, #t do
		if t[i] == elem then
			return i
		end
	end
	return 0
end

-- returns true if the table contains elem, false if not
function table.contains(t, elem)
	return (table.find(t, elem) ~= 0)
end

-- removes elem from the table
function table.remove_elem(t, elem)
	local index = table.find(t, elem)
	if index ~= 0 then
		table.remove(t, index)
	end
end

-- returns a random element of the table
function table.random(self)
	return self[crand(#self)]
end
