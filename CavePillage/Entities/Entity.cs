using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Entity : MonoBehaviour
{
    [SerializeField] protected float maxHealth = 10;
    [SerializeField] protected int maxDefense = 4;
    protected float remainingHealth;
    protected int currentDefense = 0;
    protected bool isDead;
    
    protected float damageTaken = 0;
    protected Animator anim;
    
    public bool GetIsDead()
    {
        return isDead;
    }
    public float GetHealth()
    {
        return remainingHealth;
    }
    public void TakeDamage(int amount)
    {
        float modifier =1/((float)currentDefense+1);
        Debug.Log("modifier is " + modifier);
        float toTake = amount * modifier;
        Debug.Log("toTake = " + toTake);
        damageTaken = toTake > 0 ? toTake : 0;
    }
    public void HealHealth(float amount)
    {
        float toHeal = Mathf.Min(amount, maxHealth-remainingHealth);
        remainingHealth += toHeal;
        Debug.Log("Entity healed " + toHeal + " health. Entity now has " + remainingHealth + " health");
    }
    
    protected virtual void HandleDamage()
    {
        remainingHealth -= damageTaken;
    }
    // Start is called before the first frame update
    public void Start()
    {
        remainingHealth = maxHealth;
    }
    protected virtual void Death()
    {
        isDead = true;
    }
    protected virtual void UpdateStatus()
    {
        Debug.Log("base UpdateStatus called");
    }
    // Update is called once per frame
    public void Update()
    {
        if(damageTaken != 0)
        {
            HandleDamage();
            damageTaken = 0;
        }
        if(remainingHealth <= 0 || isDead)
        {
            Death();
        }
        UpdateStatus();
    }
}
