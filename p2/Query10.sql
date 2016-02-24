select x.name
from (	select p.name, 1.00 * p.popestimate2011 / h.huest_2011 as ratio
		from pop_estimate_nation_state_pr p, housing_units_state_level h
		where p.state = h.state
		group by p.state) x,
	 (	select total(p.popestimate2011) / total(h.huest_2011) as nratio
		from pop_estimate_nation_state_pr p, housing_units_state_level h
		where p.state = h.state) y
where x.ratio < y.nratio;