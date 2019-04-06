using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Enemy : MonoBehaviour
{
    //health
    protected bool isDead = false;
    protected int damageTaken = 0;
    protected int defense;
    protected int remainingHealth;
    protected int score = 1;
    [SerializeField]protected int startingHealth=1;
    [SerializeField]protected Image healthBar;
    protected float chanceToDrop = 60f;
    
    //attack
    [SerializeField]protected float timeBetweenAttacks;     // The time in seconds between each attack.
    [SerializeField]protected int attackDamage;               // The amount of health taken away per attack.
    protected bool playerInRange;
    protected float attackTimer;
    public GameObject player;
    protected PlayerStats playerHealth;                  // Reference to the player's health.
    
    //misc
    protected EnemyType type;
    public bool GetIsDead()
    {
        return isDead;
    }
    public int GetHealth()
    {
        return remainingHealth;
    }
    public void ChangeDamageTaken(int damageToTake)
    {
        //might want to do more when an enemy takes damage
        damageTaken = damageToTake;
    }
    public void HandleDamage()
    {
        remainingHealth -= damageTaken;
        healthBar.fillAmount = (float)remainingHealth/(float)startingHealth;
    }
    
    public void TakeDamage(int amount)
    {
        int toTake = amount - defense;
        Debug.Log("enemy taking " + toTake + " damage");
        //gameObject.GetComponent<Rigidbody2D>().AddForce(knockBack);
        if(toTake > 0)
        {
            ChangeDamageTaken(toTake);
        }
    }
    public virtual void Attack ()
    {
        // Reset the timer.
        attackTimer = 0f;
        
        if(!playerHealth.GetIsDead())
        {
            playerHealth.TakeDamage(attackDamage);
        }
    }
    public virtual void CheckStatus()
    {
        if(damageTaken != 0)
        {
            HandleDamage();
            damageTaken = 0;
        }
        attackTimer += Time.deltaTime;
        if(attackTimer >= timeBetweenAttacks && playerInRange && !isDead)
        {
            Attack ();
        }
        if(remainingHealth <= 0 && !isDead)
        {
            Death();
        }
    }
    public EnemyType GetEnType()
    {
        return type;
    }
    public virtual void Death()
    {
        isDead = true;
        DropManager.Drop(gameObject, chanceToDrop);
        SpawnManager.ChangeNumEnemies(-1);
        ScoreManager.ChangeScore(score);
        Destroy(gameObject, 0.5f);
    }
    public virtual void Awake()
    {
        Debug.Log("Enemy Awakened");
        startingHealth = 1;
        defense = 1;
        remainingHealth = startingHealth;
        player =  GameObject.FindGameObjectWithTag("Player");
        playerHealth = player.GetComponent <PlayerStats> ();
        timeBetweenAttacks = 1f;
        attackDamage = 2;
    }
    // Update is called once per frame
    void Update()
    {
        Debug.Log("Enemy update called");
    }
    
    public void OnTriggerEnter2D (Collider2D other)
    {
        if(other.gameObject == player)
        {
            playerInRange = true;
        }
    }
    
    public void OnTriggerExit2D (Collider2D other)
    {
        if(other.gameObject == player)
        {
            playerInRange = false;
        }
    }
}
